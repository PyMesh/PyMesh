/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WindingNumberEngine.h"
#include <Core/Exception.h>
#ifdef WITH_IGL
#include <WindingNumber/IGL/IGLWindingNumberEngine.h>
#endif
#ifdef WITH_FAST_WINDING_NUMBER
#include <WindingNumber/FastWindingNumber/FastWindingNumberEngine.h>
#endif

#include <sstream>

using namespace PyMesh;

WindingNumberEngine::Ptr WindingNumberEngine::create(
        const std::string& engine_name) {
#ifdef WITH_IGL
    if (engine_name == "igl") {
        return std::make_shared<IGLWindingNumberEngine>();
    }
#endif
#ifdef WITH_FAST_WINDING_NUMBER
    if (engine_name == "fast_winding_number") {
        return std::make_shared<FastWindingNumberEngine>();
    }
#endif

    std::stringstream err_msg;
    err_msg << "Unknown winding number engine: " << engine_name;
    throw NotImplementedError(err_msg.str());
}
