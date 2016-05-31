/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TriangleWrapper.h"

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Triplet.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include "DimReduction.h"

#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif
#define VOID void
#define ANSI_DECLARATORS
extern "C" {
#include <triangle.h>
}

using namespace PyMesh;

namespace TriangleWrapperHelper {
    const int REGION_BOUNDARY = 2;
    std::string form_flags(Float max_area, bool split_boundary, bool refine,
            bool use_steiner_points) {
        std::stringstream flags;
        flags << "zQpa" << max_area;
        if (!split_boundary) {
            flags << "YY";
        }
        if (refine) {
            flags << "r";
        } else {
            flags << "en";
        }
        if (use_steiner_points) {
            flags << "q";
        } else {
            // By default, -S set the max number of steiner points to 0.
            flags << "S";
        }
        return flags.str();
    }

    typedef TriangleWrapper::Region Region;
    typedef std::list<Region> Regions;

    struct HashFunc {
        int operator()(const Triplet& key) const {
            return key.hash();
        }
    };
    typedef std::unordered_map<Triplet, bool, HashFunc> BoundaryMarker;

    Regions extract_regions(const MatrixIr& faces,
            const MatrixIr& face_neighbors,
            BoundaryMarker& boundary_marker) {
        const size_t num_faces = faces.rows();

        Regions regions;
        std::vector<bool> visited(num_faces, false);
        for (size_t i=0; i<num_faces; i++) {
            if (visited[i]) continue;
            Region region;
            std::list<size_t> Q;

            Q.push_back(i);
            visited[i] = true;
            while (!Q.empty()) {
                size_t face_index = Q.front();
                Q.pop_front();
                region.push_back(face_index);

                const Vector3I& face = faces.row(face_index);
                const Vector3I& adj_faces = face_neighbors.row(face_index);
                Triplet edges[3] = {
                    Triplet(face[1], face[2]),
                    Triplet(face[2], face[0]),
                    Triplet(face[0], face[1])
                };

                for (size_t j=0; j<3; j++) {
                    if (boundary_marker[edges[j]]) continue;
                    if (visited[adj_faces[j]]) continue;
                    Q.push_back(adj_faces[j]);
                    visited[adj_faces[j]] = true;
                }
            }
            regions.push_back(region);
        }
        return regions;
    }

    Float compute_winding_number(const VectorF& p,
            const MatrixFr& points,
            const MatrixIr& segments) {
        const size_t num_segments = segments.rows();
        Float total_angle=0;
        for (size_t i=0; i<num_segments; i++) {
            Vector2I segment = segments.row(i);
            Vector2F v1 = points.row(segment[0]) - p.transpose();
            Vector2F v2 = points.row(segment[1]) - p.transpose();
            Float angle = atan2(
                    v1.x()*v2.y() - v1.y()*v2.x(),
                    v1.dot(v2));
            total_angle += angle;
        }

        return total_angle / (2 * M_PI);
    }

    Float compute_2D_triangle_area(
            const VectorF& v0,
            const VectorF& v1,
            const VectorF& v2) {
        VectorF u = v1 - v0;
        VectorF v = v2 - v0;
        return u[0]*v[1] - u[1]*v[0];
    }

}

using namespace TriangleWrapperHelper;

void TriangleWrapper::run(Float max_area, bool split_boundary,
        bool auto_hole_detection, bool use_steiner_points) {
    m_max_area = max_area;
    const size_t dim = m_points.cols();
    const size_t vertex_per_segment = m_segments.cols();
    bool do_refine = (vertex_per_segment == 3);
    std::string flags = form_flags(max_area, split_boundary, do_refine,
            use_steiner_points);

    if (dim == 2) {
        process_2D_input(flags, auto_hole_detection);
    } else if (dim == 3) {
        DimReduction<3, 2> reductor(m_points);

        m_points = reductor.project(m_points);
        if (m_holes.rows() > 0) {
            assert(m_holes.cols() == 3);
            m_holes = reductor.project(m_holes);
        }
        process_2D_input(flags, auto_hole_detection);

        m_vertices = reductor.unproject(m_vertices);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

void TriangleWrapper::process_2D_input(const std::string& flags,
        bool auto_hole_detection) {
    assert(m_points.cols() == 2);
    const size_t vertex_per_segment = m_segments.cols();
    bool do_refine = (vertex_per_segment == 3);

    if (!do_refine) {
        run_triangle(m_points, m_segments, m_holes, flags);
    } else {
        refine(m_points, m_segments, flags);
    }

    if (auto_hole_detection) {
        if (do_refine) {
            throw RuntimeError(
                    "Auto hole detectiong is not necessary when refining existing triangulations.");
        } else {
            poke_holes();
        }
    }

    if (!do_refine) correct_orientation();
    else {
        // TODO
    }
}

void TriangleWrapper::run_triangle(
        const MatrixFr& points,
        const MatrixIr& segments,
        const MatrixFr& holes,
        const std::string& flags) {
    const size_t num_points = points.rows();
    const size_t dim = points.cols();
    const size_t num_segments = segments.rows();
    const size_t pt_per_segment = segments.cols();
    const size_t num_holes = holes.rows();
    if (pt_per_segment != 2) {
        throw RuntimeError("Segments must consist of 2 points!");
    }
    if (dim != 2) {
        throw RuntimeError("Triangle only works in 2D domain.");
    }

    triangulateio in, out;

    in.numberofpoints          = num_points;
    in.numberofpointattributes = 0;
    in.pointattributelist      = NULL;
    in.pointmarkerlist         = NULL;
    in.pointlist               = new REAL[num_points * dim];
    std::copy(points.data(), points.data() + num_points * dim,
            in.pointlist);

    in.trianglelist = NULL;
    in.triangleattributelist = NULL;
    in.trianglearealist = NULL;
    in.neighborlist = NULL;
    in.numberoftriangles = 0;
    in.numberofcorners = 0;
    in.numberoftriangleattributes = 0;

    in.numberofsegments = num_segments;
    in.segmentlist      = new int[num_segments * pt_per_segment];
    std::copy(segments.data(),
            segments.data() + num_segments * pt_per_segment, in.segmentlist);
    in.segmentmarkerlist = new int[num_segments];
    for (size_t i=0; i<num_segments; i++) in.segmentmarkerlist[i] = REGION_BOUNDARY;

    in.numberofholes = holes.rows();
    if (num_holes > 0) {
        assert(holes.cols() == dim);
        in.holelist = new REAL[num_holes * dim];
        std::copy(holes.data(), holes.data() + num_holes * dim,
                in.holelist);
    } else {
        in.holelist = NULL;
    }

    in.numberofregions = 0;
    in.regionlist = NULL;

    in.edgelist = NULL;
    in.edgemarkerlist = NULL;
    in.normlist = NULL;
    in.numberofedges = 0;

    out.pointlist = NULL;
    out.pointattributelist = NULL;
    out.pointmarkerlist = NULL;
    out.trianglelist = NULL;
    out.triangleattributelist = NULL;
    out.neighborlist = NULL;
    out.segmentlist = NULL;
    out.segmentmarkerlist = NULL;
    out.edgelist = NULL;
    out.edgemarkerlist = NULL;

    triangulate(const_cast<char*>(flags.c_str()), &in, &out, NULL);

    m_vertices.resize(out.numberofpoints, dim);
    std::copy(out.pointlist, out.pointlist + out.numberofpoints*dim,
            m_vertices.data());
    m_faces.resize(out.numberoftriangles, 3);
    std::copy(out.trianglelist, out.trianglelist + out.numberoftriangles*3,
            m_faces.data());
    m_face_neighbors.resize(out.numberoftriangles, 3);
    std::copy(out.neighborlist, out.neighborlist + out.numberoftriangles*3,
            m_face_neighbors.data());
    m_edges.resize(out.numberofedges * 2);
    std::copy(out.edgelist, out.edgelist + out.numberofedges * 2,
            m_edges.data());
    m_edge_marks.resize(out.numberofedges);
    std::copy(out.edgemarkerlist, out.edgemarkerlist + out.numberofedges,
            m_edge_marks.data());

    if (num_points > 0) delete [] in.pointlist;
    if (num_segments > 0) delete [] in.segmentlist;
    if (num_segments > 0) delete [] in.segmentmarkerlist;
    if (num_holes > 0) delete [] in.holelist;

    if (out.numberofpoints > 0) delete [] out.pointlist;
    if (out.numberofpointattributes > 0) delete [] out.pointattributelist;
    if (out.numberoftriangles > 0) delete [] out.trianglelist;
    if (out.numberoftriangles > 0) delete [] out.neighborlist;
    if (out.numberofsegments > 0) delete [] out.segmentlist;
    if (out.numberofsegments > 0) delete [] out.segmentmarkerlist;
    if (out.numberofedges > 0) delete [] out.edgelist;
    if (out.numberofedges > 0) delete [] out.edgemarkerlist;
}

void TriangleWrapper::refine(
        const MatrixFr& points,
        const MatrixIr& faces,
        const std::string& flags) {
    const size_t num_points = points.rows();
    const size_t dim = points.cols();
    const size_t num_faces = faces.rows();
    const size_t pt_per_face = faces.cols();
    if (pt_per_face != 3) {
        throw RuntimeError("Face must consist of 3 points!");
    }
    if (dim != 2) {
        throw RuntimeError("Triangle only works in 2D domain.");
    }

    triangulateio in, out;

    in.numberofpoints          = num_points;
    in.numberofpointattributes = 0;
    in.pointattributelist      = NULL;
    in.pointmarkerlist         = NULL;
    in.pointlist               = new REAL[num_points * dim];
    std::copy(points.data(), points.data() + num_points * dim,
            in.pointlist);

    in.numberoftriangles = num_faces;
    in.trianglelist      = new int[num_faces * pt_per_face];
    std::copy(faces.data(), faces.data() + num_faces * pt_per_face,
            in.trianglelist);
    in.numberoftriangleattributes = 0;
    in.numberofcorners = 3;

    in.numberofholes    = 0;
    in.numberofsegments = 0;
    in.numberofregions  = 0;

    out.pointlist = NULL;
    out.pointattributelist = NULL;
    out.pointmarkerlist = NULL;
    out.trianglelist = NULL;
    out.triangleattributelist = NULL;
    out.neighborlist = NULL;
    out.segmentlist = NULL;
    out.segmentmarkerlist = NULL;
    out.edgelist = NULL;
    out.edgemarkerlist = NULL;

    triangulate(const_cast<char*>(flags.c_str()), &in, &out, NULL);

    m_vertices.resize(out.numberofpoints, dim);
    std::copy(out.pointlist, out.pointlist + out.numberofpoints*dim,
            m_vertices.data());
    m_faces.resize(out.numberoftriangles, 3);
    std::copy(out.trianglelist, out.trianglelist + out.numberoftriangles*3,
            m_faces.data());

    if (num_points > 0) delete [] in.pointlist;
    if (num_faces > 0) delete [] in.trianglelist;

    if (out.numberofpoints > 0) delete [] out.pointlist;
    if (out.numberofpointattributes > 0) delete [] out.pointattributelist;
    if (out.numberoftriangles > 0) delete [] out.trianglelist;
    if (out.numberofsegments > 0) delete [] out.segmentlist;
    if (out.numberofedges > 0) delete [] out.edgelist;
    if (out.numberofedges > 0) delete [] out.edgemarkerlist;
}

void TriangleWrapper::poke_holes() {
    const size_t num_faces = m_faces.rows();
    const size_t num_edges = m_edge_marks.size();
    assert(m_edges.size() == num_edges * 2);
    BoundaryMarker boundary_markers;

    for (size_t i=0; i<num_edges; i++) {
        Triplet edge(m_edges[i*2], m_edges[i*2+1]);
        boundary_markers.insert(
                std::make_pair(edge, m_edge_marks[i] == REGION_BOUNDARY));
    }

    Regions regions = extract_regions(
            m_faces, m_face_neighbors, boundary_markers);

    std::list<size_t> interior_faces;
    for (auto& region : regions) {
        VectorF seed_p;
        bool seed_found = select_seed_point(region, seed_p);

        if (!seed_found) {
            // All faces are degenerated in this region.
            continue;
        }

        Float wind_num = compute_winding_number(seed_p, m_points, m_segments);
        if (fabs(wind_num) > 0.5) {
            interior_faces.splice(interior_faces.end(), region);
        }
    }

    MatrixIr faces(interior_faces.size(), 3);
    size_t count=0;
    for (const auto& f_index : interior_faces) {
        faces.row(count) = m_faces.row(f_index);
        count++;
    }

    IsolatedVertexRemoval remover(m_vertices, faces);
    remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

bool TriangleWrapper::select_seed_point(const Region& region, VectorF& seed_p) {
    const Float degeneracy_tol = std::min(m_max_area * 0.01, 1e-6);
    bool seed_found = false;
    for (auto face_idx : region) {
        const VectorI& f = m_faces.row(face_idx);
        const VectorF& v0 = m_vertices.row(f[0]);
        const VectorF& v1 = m_vertices.row(f[1]);
        const VectorF& v2 = m_vertices.row(f[2]);
        Float area = compute_2D_triangle_area(v0, v1, v2);
        if (area > degeneracy_tol) {
            seed_found = true;
            seed_p = (v0+v1+v2)/3.0;
            break;
        }
    }
    return seed_found;
}

void TriangleWrapper::correct_orientation() {
    const size_t num_faces = m_faces.rows();
    if (num_faces == 0) return;

    Region region;
    for (size_t i=0; i<num_faces; i++) region.push_back(i);

    VectorF seed_p;
    bool seed_found = select_seed_point(region, seed_p);
    if (!seed_found) return;

    Float wind_num = compute_winding_number(seed_p, m_points, m_segments);
    if (wind_num < -0.5) {
        m_faces.col(1).swap(m_faces.col(2));
    }
}

