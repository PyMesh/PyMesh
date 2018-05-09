/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */

#include "BVHEngine.h"

#if WITH_CGAL
#include "CGAL/AABBTree.h"
#endif
#if WITH_GEOGRAM
#include "Geogram/AABBTree.h"
#endif

using namespace PyMesh;

BVHEngine::Ptr BVHEngine::create(const std::string& engine_name) {
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
    throw NotImplementedError("BVH Engine (" + engine_name
            + ") is not supported.");
}
