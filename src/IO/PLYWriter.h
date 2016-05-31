/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Mesh.h>

#include <string>
#include <vector>

#include "MeshWriter.h"
#include "rply.h"

namespace PyMesh {

class PLYWriter : public MeshWriter {
    public:
        PLYWriter() : m_in_ascii(false), m_scalar(PLY_DOUBLE) {}
        virtual ~PLYWriter() {}

    public:
        virtual void with_attribute(const std::string& attr_name);
        virtual void in_ascii() { m_in_ascii=true; }
        virtual void use_float() { m_scalar = PLY_FLOAT; }
        virtual void write_mesh(Mesh& mesh);
        virtual void write(
                const VectorF& vertices,
                const VectorI& faces, 
                const VectorI& voxels,
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
        e_ply_type m_scalar;
};

}
