/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <list>

#include "MeshWriter.h"

namespace PyMesh {

class MshSaver;

class MSHWriter : public MeshWriter {
    public:
        MSHWriter() : m_in_ascii(false) {}
        virtual ~MSHWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void in_ascii() { m_in_ascii=true; }
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces,
                const VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);

    private:
        void write_volume_mesh(Mesh& mesh);
        void write_surface_mesh(Mesh& mesh);
        void write_attribute(MshSaver& saver, const std::string& name,
                VectorF& value, size_t dim, size_t num_vertices, size_t num_elements);

    private:
        typedef std::list<std::string> AttrNames;
        AttrNames m_attr_names;
        bool m_in_ascii;
};

}
