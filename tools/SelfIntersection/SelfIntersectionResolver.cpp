/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SelfIntersectionResolver.h"

#include <sstream>
#include <Core/Exception.h>

#ifdef WITH_IGL
#include <IGL/IGLSelfIntersectionResolver.h>
#endif

using namespace PyMesh;

SelfIntersectionResolver::Ptr SelfIntersectionResolver::create(
        const std::string& engine_name) {
#ifdef WITH_IGL
    if (engine_name == "igl") {
        return Ptr(new IGLSelfIntersectionResolver);
    }
#endif

    std::stringstream err_msg;
    err_msg << "Self-intersection engine \"" << engine_name
        << "\" is not supported";
    throw NotImplementedError(err_msg.str());
}
