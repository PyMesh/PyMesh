/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "BVHEngine.h"

#if WITH_CGAL
#include "CGAL/AABBTree.h"
#endif
#if WITH_GEOGRAM
#include "Geogram/AABBTree.h"
#endif
#if WITH_IGL
#include "IGL/AABBTree.h"
#endif

using namespace PyMesh;

BVHEngine::Ptr BVHEngine::create(const std::string& engine_name) {
    if (engine_name == "auto") {
#if WITH_CGAL
        return BVHEngine::create("cgal");
#elif WITH_GEOGRAM
        return BVHEngine::create("geogram");
#elif WITH_IGL
        return BVHEngine::create("igl");
#endif
    }

#if WITH_CGAL
    if (engine_name == "cgal") {
        return std::make_shared<CGAL::AABBTree>();
    }
#endif
#if WITH_GEOGRAM
    if (engine_name == "geogram") {
        return std::make_shared<Geogram::AABBTree>();
    }
#endif
#if WITH_IGL
    if (engine_name == "igl") {
        return std::make_shared<IGL::AABBTree>();
    }
#endif
    throw NotImplementedError("BVH Engine (" + engine_name
            + ") is not supported.");
}

std::vector<std::string> BVHEngine::get_available_engines() {
    std::vector<std::string> engine_names;
#if WITH_CGAL
    engine_names.push_back("cgal");
#endif
#if WITH_GEOGRAM
    engine_names.push_back("geogram");
#endif
#if WITH_IGL
    engine_names.push_back("igl");
#endif
    return engine_names;
}

