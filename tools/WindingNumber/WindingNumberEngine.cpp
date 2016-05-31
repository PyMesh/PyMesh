/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WindingNumberEngine.h"
#include <Core/Exception.h>
#ifdef WITH_IGL
#include <IGL/IGLWindingNumberEngine.h>
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

    std::stringstream err_msg;
    err_msg << "Unknown winding number engine: " << engine_name;
    throw NotImplementedError(err_msg.str());
}
