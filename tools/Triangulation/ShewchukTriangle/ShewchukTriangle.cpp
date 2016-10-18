/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "ShewchukTriangle.h"
#include <Core/Exception.h>

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

void ShewchukTriangle::run() {
    const size_t num_points = m_vertices.rows();
    const size_t dim = m_vertices.cols();
    if (dim != 2) {
        throw RuntimeError("Triangle only works in 2D domain.");
    }

    triangulateio in, out;

    in.numberofpoints          = num_points;
    in.numberofpointattributes = 0;
    in.pointattributelist      = NULL;
    in.pointmarkerlist         = NULL;
    in.pointlist               = new REAL[num_points * dim];
    std::copy(m_vertices.data(),
            m_vertices.data() + num_points * dim,
            in.pointlist);

    in.trianglelist = NULL;
    in.triangleattributelist = NULL;
    in.trianglearealist = NULL;
    in.neighborlist = NULL;
    in.numberoftriangles = 0;
    in.numberofcorners = 0;
    in.numberoftriangleattributes = 0;

    in.numberofsegments = 0;
    in.segmentlist      = NULL;

    in.numberofholes = 0;
    in.holelist = NULL;

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

    std::string flags("Q");
    triangulate(const_cast<char*>(flags.c_str()), &in, &out, NULL);

    m_faces.resize(out.numberoftriangles, 3);
    std::transform(out.trianglelist, out.trianglelist + out.numberoftriangles*3,
            m_faces.data(), [](size_t index){ return index-1; });

    if (num_points > 0) delete [] in.pointlist;

    if (out.numberofpoints > 0) delete [] out.pointlist;
    if (out.numberofpointattributes > 0) delete [] out.pointattributelist;
    if (out.numberoftriangles > 0) delete [] out.trianglelist;
    if (out.numberoftriangles > 0) delete [] out.neighborlist;
    if (out.numberofsegments > 0) delete [] out.segmentlist;
    if (out.numberofsegments > 0) delete [] out.segmentmarkerlist;
    if (out.numberofedges > 0) delete [] out.edgelist;
    if (out.numberofedges > 0) delete [] out.edgemarkerlist;
}

