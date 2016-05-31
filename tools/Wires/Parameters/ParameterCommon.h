/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <map>
#include <string>
#include <Core/EigenTypedef.h>

namespace PyMesh {
namespace ParameterCommon {
    enum TargetType { VERTEX, EDGE };
    typedef std::map<std::string, Float> Variables;
}
}
