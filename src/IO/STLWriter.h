/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Mesh.h>
#include "MeshWriter.h"

namespace PyMesh {
class STLWriter : public MeshWriter {
    public:
        STLWriter() : m_in_ascii(false) {}
        virtual ~STLWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void in_ascii() { m_in_ascii=true; }
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim,
                size_t vertex_per_face,
                size_t vertex_per_voxel);

    private:
        void check_mesh(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim,
                size_t vertex_per_face,
                size_t vertex_per_voxel) const;
        void write_ascii(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim,
                size_t vertex_per_face,
                size_t vertex_per_voxel);

        void write_binary(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim,
                size_t vertex_per_face,
                size_t vertex_per_voxel);

    private:
        bool m_in_ascii;
};

}
