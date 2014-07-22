#pragma once

#include <Mesh.h>

#include <string>
#include <vector>

#include "MeshWriter.h"
#include "rply.h"

class PLYWriter : public MeshWriter {
    public:
        PLYWriter() : m_in_ascii(false) {}
        virtual ~PLYWriter() {}

    public:
        virtual MeshWriter& with_attribute(const std::string& attr_name);
        virtual MeshWriter& in_ascii() { m_in_ascii=true; return *this; }
        virtual void write_mesh(Mesh& mesh);
        virtual void write(VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel);

    protected:
        void regroup_attribute_names(Mesh& mesh);

        void add_vertex_elements_header(Mesh& mesh, p_ply& ply);
        void add_face_elements_header(Mesh& mesh, p_ply& ply);
        void add_voxel_elements_header(Mesh& mesh, p_ply& ply);

        void write_vertex_elements(Mesh& mesh, p_ply& ply);
        void write_face_elements(Mesh& mesh, p_ply& ply);
        void write_voxel_elements(Mesh& mesh, p_ply& ply);

    protected:
        typedef std::vector<std::string> NameArray;

        NameArray m_attr_names;
        NameArray m_vertex_attr_names;
        NameArray m_face_attr_names;
        NameArray m_voxel_attr_names;

        bool m_in_ascii;
};
