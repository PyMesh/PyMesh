#pragma once
#include <string>

#include "EigenTypedef.h"

class Mesh;

namespace Zhou {
    VectorF convert_to_vertex_attribute(
            Mesh& mesh, const VectorF& attribute);

    VectorF convert_to_vertex_attribute(
            Mesh& mesh, const std::string& attribute_name);
}
