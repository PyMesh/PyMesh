/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "Triangulation.h"

#ifdef WITH_IGL
#include "IGL/LexicographicTriangulation.h"
#include "IGL/DelaunayTriangulation.h"
#endif

#ifdef WITH_CGAL
#include "CGAL/CGALDelaunayTriangulation.h"
#endif

#ifdef WITH_TRIANGLE
#include "ShewchukTriangle/ShewchukTriangle.h"
#endif

#ifdef WITH_GEOGRAM
#include "GeoGram/GeoGramDelaunayTriangulation.h"
#endif

#include <Core/Exception.h>
#include <sstream>

using namespace PyMesh;

Triangulation::Ptr Triangulation::create(const std::string& engine_name) {
#ifdef WITH_IGL
    if (engine_name == "igl_lexicographic") {
        return Ptr(new LexicographicTriangulation());
    }
#endif

#ifdef WITH_IGL
    if (engine_name == "igl_delaunay") {
        return Ptr(new DelaunayTriangulation());
    }
#endif

#ifdef WITH_TRIANGLE
    if (engine_name == "shewchuk_triangle") {
        return Ptr(new ShewchukTriangle());
    }
#endif

#ifdef WITH_CGAL
    if (engine_name == "cgal_delaunay") {
        return Ptr(new CGALDelaunayTriangulation());
    }
#endif

#ifdef WITH_GEOGRAM
    if (engine_name == "geogram_delaunay") {
        return Ptr(new GeoGramDelaunayTriangulation());
    }
#endif

    std::stringstream err_msg;
    err_msg << "Triangulation engine " << engine_name
        << " is not supported";
    throw NotImplementedError(err_msg.str());
}
