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

BVHEngine::Ptr BVHEngine::create(const std::string& engine_name, size_t dim) {
    if (engine_name == "auto") {
#if WITH_IGL
        return BVHEngine::create("igl", dim);
#elif WITH_CGAL
        if (dim == 3) return BVHEngine::create("cgal", dim);
#elif WITH_GEOGRAM
        if (dim == 3) return BVHEngine::create("geogram", dim);
#endif
    }

#if WITH_CGAL
    if (engine_name == "cgal") {
        if (dim != 3) {
            throw NotImplementedError(
                    "Only 3D meshes are supported by CGAL BVH");
        }
        return std::make_shared<_CGAL::AABBTree>();
    }
#endif
#if WITH_GEOGRAM
    if (engine_name == "geogram") {
        if (dim != 3) {
            throw NotImplementedError(
                    "Only 3D meshes are supported by Geogram BVH");
        }
        return std::make_shared<Geogram::AABBTree>();
    }
#endif
#if WITH_IGL
     if (engine_name == "igl") {
         switch (dim) {
             case 2:
                 return std::make_shared<IGL::AABBTree<2>>();
             case 3:
                 return std::make_shared<IGL::AABBTree<3>>();
             default:
                 throw NotImplementedError("Only 2D and 3D meshes are supported");
         }
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

