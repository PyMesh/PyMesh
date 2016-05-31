/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Subdivision.h"
#include "SimpleSubdivision.h"
#include "LoopSubdivision.h"

#include <sstream>

#include <Core/Exception.h>

using namespace PyMesh;

Subdivision::Ptr Subdivision::create(const std::string& type) {
    if (type == "simple") {
        return Ptr(new SimpleSubdivision());
    } else if (type == "loop") {
        return Ptr(new LoopSubdivision());
    } else {
        std::stringstream err_msg;
        err_msg << "Invalid subdivision type: " << type;
        throw NotImplementedError(err_msg.str());
    }
}
