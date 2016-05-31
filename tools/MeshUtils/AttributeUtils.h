/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class Mesh;

namespace AttributeUtils {
    VectorF convert_to_vertex_attribute(
            Mesh& mesh, const VectorF& attribute);

    VectorF convert_to_vertex_attribute_from_name(
            Mesh& mesh, const std::string& attribute_name);

    VectorF convert_to_face_attribute(
            Mesh& mesh, const VectorF& attribute);

    VectorF convert_to_face_attribute_from_name(
            Mesh& mesh, const std::string& attribute_name);

    VectorF convert_to_voxel_attribute(
            Mesh& mesh, const VectorF& attribute);

    VectorF convert_to_voxel_attribute_from_name(
            Mesh& mesh, const std::string& attribute_name);
}

}
