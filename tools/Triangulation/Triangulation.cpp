/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */

#include "Triangulation.h"
#if WITH_IGL
#include "IGL/LexicographicTriangulation.h"
#include "IGL/DelaunayTriangulation.h"
#endif
#if WITH_TRIANGLE
#include "ShewchukTriangle/ShewchukTriangle.h"
#endif
#include <Core/Exception.h>
#include <sstream>

using namespace PyMesh;

Triangulation::Ptr Triangulation::create(const std::string& engine_name) {
#if WITH_IGL
    if (engine_name == "igl_lexicographic") {
        return Ptr(new LexicographicTriangulation());
    }
#endif

#if WITH_IGL
    if (engine_name == "igl_delaunay") {
        return Ptr(new DelaunayTriangulation());
    }
#endif

#if WITH_TRIANGLE
    if (engine_name == "shewchuk_triangle") {
        return Ptr(new ShewchukTriangle());
    }
#endif

    std::stringstream err_msg;
    err_msg << "Triangulation engine " << engine_name
        << " is not supported";
    throw NotImplementedError(err_msg.str());
}
