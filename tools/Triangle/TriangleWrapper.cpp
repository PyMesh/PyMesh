/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#ifdef WITH_TRIANGLE
#include "TriangleWrapper.h"

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <Core/Exception.h>
#include <Misc/Multiplet.h>
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
    using Region = TriangleWrapper::Region;
    using Regions = TriangleWrapper::Regions;

    struct HashFunc {
        int operator()(const Duplet& key) const {
            return key.hash();
        }
    };
    typedef std::unordered_map<Duplet, bool, HashFunc> BoundaryMarker;

    Regions extract_regions(const MatrixIr& faces,
            const MatrixIr& face_neighbors,
            const VectorI& edges,
            const VectorI& edge_marks) {
        const size_t num_edges = edge_marks.size();
        assert(edges.size() == num_edges * 2);
        BoundaryMarker boundary_markers;

        for (size_t i=0; i<num_edges; i++) {
            Duplet edge(edges[i*2], edges[i*2+1]);
            boundary_markers.insert(
                    std::make_pair(edge, edge_marks[i] == REGION_BOUNDARY));
        }

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
                Duplet edges[3] = {
                    {face[1], face[2]},
                    {face[2], face[0]},
                    {face[0], face[1]}
                };

                for (size_t j=0; j<3; j++) {
                    if (boundary_markers[edges[j]]) continue;
                    if (adj_faces[j] < 0) continue; // adj to boundary.
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

void TriangleWrapper::run() {
    const size_t dim = m_points.cols();
    std::string flags = generate_command_line_options();

    if (dim == 2) {
        process_2D_input(flags);
    } else if (dim == 3) {
        DimReduction<3, 2> reductor(m_points);

        m_points = reductor.project(m_points);
        if (m_holes.rows() > 0) {
            assert(m_holes.cols() == 3);
            m_holes = reductor.project(m_holes);
        }
        process_2D_input(flags);

        m_vertices = reductor.unproject(m_vertices);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

std::string TriangleWrapper::generate_command_line_options() const {
    // Basic flag:
    //   z: index starts from zero.
    //   n: output triangle neighbor info.
    //   e: output edge list.
    std::string opt = "zne";

    if (m_points.rows() == 0) {
        throw RuntimeError("Empty input detected for triangulation");
    } else if (m_segments.rows() > 0) {
        opt += "p"; // Triangulate PSLG.
    } else if (m_triangles.rows() > 0) {
        opt += "r"; // Refinement.
    } else {
        opt += "v"; // Also compute Voronoi diagram.
    }

    if (m_min_angle > 0.0) {
        if (m_min_angle > 34) {
            std::cerr << "Warning: min angle > 34 degrees may cause algorithm to not terminate" 
                << std::endl;
        } else if (m_min_angle > 20.7) {
            std::cerr << "Warning: Theoretical guarentee of termination is lost "
                << "for min angle > 20.7 degrees.  "
                << "But in practice, it often succeed for min angle >= 33 degrees."
                << std::endl;
        }
        opt += "q" + std::to_string(m_min_angle);
    } else if (m_min_angle < 0.0) {
        std::cerr << "Warning: min angle < 0 degrees.  Ignored." << std::endl;
    }

    if (m_max_area > 0) {
        opt += "a" + std::to_string(m_max_area);
    } else if (m_max_areas.size() > 0) {
        assert(m_max_areas.size() == m_triangles.rows());
        opt += "a";
    }
    if (m_convex_hull) {
        opt += "c";
    }
    if (m_conforming) {
        opt += "D";
    }
    if (!m_exact) {
        opt += "X";
    }
    if (!m_split_boundary) {
        opt += "Y";
    }
    if (m_max_num_steiner >= 0){
        opt += "S" + std::to_string(m_max_num_steiner);
    }
    switch (m_verbose_level) {
        case 0:
            opt += "Q";
            break;
        case 1:
            break;
        case 2:
            opt += "V";
            break;
        case 3:
            opt += "VV";
            break;
        case 4:
            opt += "VVVV";
            break;
        default:
            throw NotImplementedError("Unknown verbose level: "
                    + std::to_string(m_verbose_level));
    }
    switch (m_algorithm) {
        case Algorithm::DIVIDE_AND_CONQUER:
            break;
        case Algorithm::SWEEPLINE:
            opt += "F";
            break;
        case Algorithm::INCREMENTAL:
            opt += "i";
            break;
        default:
            throw NotImplementedError("Unknown triangulation algorithm");
    }
    return opt;
}

void TriangleWrapper::process_2D_input(const std::string& flags) {
    assert(m_points.cols() == 2);

    run_triangle(flags);

    if (m_segments.rows() > 0) {
        auto regions = extract_regions(
                m_faces, m_face_neighbors, m_edges, m_edge_marks);
        m_regions = VectorI::Zero(m_faces.rows());
        size_t count = 1;
        for (const auto& r : regions) {
            for (const auto& i : r) {
                m_regions[i] = count;
            }
            count++;
        }

        if (m_auto_hole_detection) {
            poke_holes(regions);
        }
    } else {
        m_regions = VectorI::Zero(m_faces.rows());
    }

    correct_orientation();
}

void TriangleWrapper::run_triangle(const std::string& flags) {
    const size_t num_points = m_points.rows();
    const size_t dim = m_points.cols();
    const size_t num_segments = m_segments.rows();
    const size_t pt_per_segment = m_segments.cols();
    const size_t num_holes = m_holes.rows();
    const size_t num_triangles = m_triangles.rows();
    const size_t pt_per_triangle = m_triangles.cols();
    const size_t num_areas = m_max_areas.size();

    if (num_segments > 0 && pt_per_segment != 2) {
        throw RuntimeError("Segments must consist of 2 points!");
    }
    if (num_triangles > 0 && pt_per_triangle != 3) {
        throw RuntimeError("Triangles must consists of 3 points!");
    }
    if (dim != 2) {
        throw RuntimeError("Triangle only works in 2D domain.");
    }

    auto generate_empty_triangulateio = []() {
        triangulateio io;
        io.numberofpoints = 0;
        io.numberofpointattributes = 0;
        io.numberoftriangles = 0;
        io.numberoftriangleattributes = 0;
        io.numberofcorners = 0;
        io.numberofsegments = 0;
        io.numberofholes = 0;
        io.numberofregions = 0;
        io.numberofedges = 0;
        io.pointlist = nullptr;
        io.pointattributelist = nullptr;
        io.pointmarkerlist = nullptr;
        io.trianglelist = nullptr;
        io.triangleattributelist = nullptr;
        io.trianglearealist = nullptr;
        io.neighborlist = nullptr;
        io.segmentlist = nullptr;
        io.segmentmarkerlist = nullptr;
        io.edgelist = nullptr;
        io.edgemarkerlist = nullptr;
        io.holelist = nullptr;
        io.regionlist = nullptr;
        io.normlist = nullptr;
        return io;
    };

    triangulateio in       = generate_empty_triangulateio();
    triangulateio out      = generate_empty_triangulateio();
    triangulateio out_voro = generate_empty_triangulateio();

    in.numberofpoints          = num_points;
    in.numberofpointattributes = 0;
    in.pointlist               = new REAL[num_points * dim];
    std::copy(m_points.data(), m_points.data() + num_points * dim,
            in.pointlist);

    if (num_triangles > 0) {
        in.trianglelist = new int[num_triangles * pt_per_triangle];
        std::copy(m_triangles.data(),
                m_triangles.data() + num_triangles * pt_per_triangle,
                in.trianglelist);
        in.numberoftriangles = num_triangles;
        in.numberofcorners = 3;
    }

    if (num_segments > 0) {
        in.numberofsegments = num_segments;
        in.segmentlist = new int[num_segments * pt_per_segment];
        std::copy(m_segments.data(),
                m_segments.data() + num_segments * pt_per_segment,
                in.segmentlist);
        in.segmentmarkerlist = new int[num_segments];
        for (size_t i=0; i<num_segments; i++) in.segmentmarkerlist[i] = REGION_BOUNDARY;
    }

    in.numberofholes = m_holes.rows();
    if (num_holes > 0) {
        assert(m_holes.cols() == dim);
        in.holelist = new REAL[num_holes * dim];
        std::copy(m_holes.data(), m_holes.data() + num_holes * dim,
                in.holelist);
    }

    if (num_areas > 0) {
        assert(num_areas == num_triangles);
        in.trianglearealist = new REAL[num_areas];
        std::copy(m_max_areas.data(), m_max_areas.data() + num_areas,
                in.trianglearealist);
    }

    triangulate(const_cast<char*>(flags.c_str()), &in, &out, &out_voro);

    if (out.numberofpoints > 0 && out.pointlist) {
        m_vertices.resize(out.numberofpoints, dim);
        std::copy(out.pointlist, out.pointlist + out.numberofpoints*dim,
                m_vertices.data());
    }
    if (out.numberoftriangles > 0 && out.trianglelist) {
        m_faces.resize(out.numberoftriangles, 3);
        std::copy(out.trianglelist, out.trianglelist + out.numberoftriangles*3,
                m_faces.data());
    }
    if (out.numberoftriangles > 0 && out.neighborlist) {
        m_face_neighbors.resize(out.numberoftriangles, 3);
        std::copy(out.neighborlist, out.neighborlist + out.numberoftriangles*3,
                m_face_neighbors.data());
    }
    if (out.numberofedges > 0 && out.edgelist) {
        m_edges.resize(out.numberofedges * 2);
        std::copy(out.edgelist, out.edgelist + out.numberofedges * 2,
                m_edges.data());
    }
    if (out.numberofedges > 0 && out.edgemarkerlist) {
        m_edge_marks.resize(out.numberofedges);
        std::copy(out.edgemarkerlist, out.edgemarkerlist + out.numberofedges,
                m_edge_marks.data());
    }

    if (out_voro.numberofpoints > 0 && out_voro.pointlist) {
        m_voronoi_vertices.resize(out_voro.numberofpoints, dim);
        std::copy(out_voro.pointlist,
                out_voro.pointlist + out_voro.numberofpoints*dim,
                m_voronoi_vertices.data());
    }
    if (out_voro.numberofedges > 0 && out_voro.edgelist) {
        m_voronoi_edges.resize(out_voro.numberofedges, 2);
        std::copy(out_voro.edgelist,
                out_voro.edgelist + out_voro.numberofedges * 2,
                m_voronoi_edges.data());
    }

    if (num_points > 0) delete [] in.pointlist;
    if (num_segments > 0) {
        delete [] in.segmentlist;
        delete [] in.segmentmarkerlist;
    }
    if (num_triangles > 0) delete [] in.trianglelist;
    if (num_holes > 0) delete [] in.holelist;
    if (num_areas > 0) delete [] in.trianglearealist;

    if (out.numberofpoints > 0) delete [] out.pointlist;
    if (out.numberofpointattributes > 0) delete [] out.pointattributelist;
    if (out.numberoftriangles > 0) delete [] out.trianglelist;
    if (out.numberoftriangles > 0) delete [] out.neighborlist;
    if (out.numberofsegments > 0) delete [] out.segmentlist;
    if (out.numberofsegments > 0) delete [] out.segmentmarkerlist;
    if (out.numberofedges > 0) delete [] out.edgelist;
    if (out.numberofedges > 0) delete [] out.edgemarkerlist;

    if (out_voro.numberofpoints > 0) delete [] out_voro.pointlist;
    if (out_voro.numberofpointattributes > 0) delete [] out_voro.pointattributelist;
    if (out_voro.numberoftriangles > 0) delete [] out_voro.trianglelist;
    if (out_voro.numberoftriangles > 0) delete [] out_voro.neighborlist;
    if (out_voro.numberofsegments > 0) delete [] out_voro.segmentlist;
    if (out_voro.numberofsegments > 0) delete [] out_voro.segmentmarkerlist;
    if (out_voro.numberofedges > 0) delete [] out_voro.edgelist;
    if (out_voro.numberofedges > 0) delete [] out_voro.edgemarkerlist;
}

void TriangleWrapper::poke_holes(Regions& regions) {
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
    VectorI interior_regions(interior_faces.size());
    size_t count=0;
    for (const auto& f_index : interior_faces) {
        faces.row(count) = m_faces.row(f_index);
        interior_regions[count] = m_regions[f_index];
        count++;
    }

    IsolatedVertexRemoval remover(m_vertices, faces);
    remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();

    m_regions.swap(interior_regions);
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

#endif
