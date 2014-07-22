#pragma once

#include <string>
#include <list>

#include "MeshWriter.h"

class MshSaver;

class MSHWriter : public MeshWriter {
    public:
        MSHWriter() : m_in_ascii(false) {}
        virtual ~MSHWriter() {}

    public:
        virtual MeshWriter& with_attribute(const std::string& attr_name);
        virtual MeshWriter& in_ascii() { m_in_ascii=true; return *this; }
        virtual void write_mesh(Mesh& mesh);
        virtual void write(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);

    private:
        void write_volume_mesh(Mesh& mesh);
        void write_surface_mesh(Mesh& mesh);
        void write_geometry(MshSaver& saver, VectorF& vertices,
                VectorI& elements, size_t dim, size_t vertex_per_element);
        void write_attribute(MshSaver& saver, const std::string& name,
                VectorF& value, size_t dim, size_t num_vertices, size_t num_elements);

    private:
        typedef std::list<std::string> AttrNames;
        AttrNames m_attr_names;
        bool m_in_ascii;
};
