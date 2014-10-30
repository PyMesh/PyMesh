#include "TriangleWrapper.h"

#include <iostream>
#include <sstream>

#include <Core/Exception.h>
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

namespace TriangleWrapperHelper {
    std::string form_flags(Float max_area, bool split_boundary) {
        std::stringstream flags;
        flags << "zQpea" << max_area;
        if (!split_boundary) {
            flags << "Y";
        }
        return flags.str();
    }
}

using namespace TriangleWrapperHelper;

void TriangleWrapper::run(Float max_area, bool split_boundary) {
    std::string flags = form_flags(max_area, split_boundary);
    const size_t dim = m_points.cols();
    const size_t vertex_per_segment = m_segments.cols();
    if (dim == 2) {
        if (vertex_per_segment == 2) {
            run_triangle(m_points, m_segments, m_holes, flags);
        } else if (vertex_per_segment == 3) {
            flags += "r";
            refine(m_points, m_segments, flags);
        } else {
            throw RuntimeError("Unsupported segment type");
        }
    } else if (dim == 3) {
        DimReduction<3, 2> reductor(m_points);

        MatrixFr pts = reductor.project(m_points);
        MatrixFr holes;
        if (m_holes.rows() > 0) {
            assert(m_holes.cols() == 3);
            holes = reductor.project(m_holes);
        }

        if (vertex_per_segment == 2) {
            run_triangle(pts, m_segments, holes, flags);
        } else if (vertex_per_segment == 3) {
            flags += "r";
            refine(pts, m_segments, flags);
        } else {
            throw RuntimeError("Unsupported segment type");
        }

        m_vertices = reductor.unproject(m_vertices);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
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

    in.numberofsegments = num_segments;
    in.segmentlist      = new int[num_segments * pt_per_segment];
    std::copy(segments.data(),
            segments.data() + num_segments * pt_per_segment, in.segmentlist);
    in.segmentmarkerlist = NULL;

    in.numberofholes = holes.rows();
    if (num_holes > 0) {
        assert(holes.cols() == dim);
        in.holelist = new REAL[num_holes * dim];
        std::copy(holes.data(), holes.data() + num_holes * dim,
                in.holelist);
    }

    in.numberofregions = 0;

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
    if (num_segments > 0) delete [] in.segmentlist;
    if (num_holes > 0) delete [] in.holelist;

    if (out.numberofpoints > 0) delete [] out.pointlist;
    if (out.numberofpointattributes > 0) delete [] out.pointattributelist;
    if (out.numberoftriangles > 0) delete [] out.trianglelist;
    if (out.numberofsegments > 0) delete [] out.segmentlist;
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
