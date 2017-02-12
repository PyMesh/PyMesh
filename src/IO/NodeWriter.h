/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>

#include <Mesh.h>
#include "MeshWriter.h"

namespace PyMesh {

class NodeWriter : public MeshWriter {
    public:
        NodeWriter() :
            m_with_region_attribute(false),
            m_with_node_bd_marker(false),
            m_with_face_bd_marker(false) {}
        virtual ~NodeWriter() = default;

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);

    private:
        void write_node_file(const std::string& filename, Mesh& mesh);
        void write_face_file(const std::string& filename, Mesh& mesh);
        void write_elem_file(const std::string& filename, Mesh& mesh);

    private:
        bool m_with_region_attribute;
        bool m_with_node_bd_marker;
        bool m_with_face_bd_marker;
};

}
