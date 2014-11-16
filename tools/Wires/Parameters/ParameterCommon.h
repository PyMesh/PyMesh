#pragma once
#include <map>
#include <string>
#include <Core/EigenTypedef.h>

namespace ParameterCommon {
    enum TargetType { VERTEX, EDGE };
    typedef std::map<std::string, Float> Variables;
}
