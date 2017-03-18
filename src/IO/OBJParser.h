/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshParser.h"

#include <list>
#include <vector>
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class OBJParser : public MeshParser {
    // TODO: only triangular mesh is supported
    public:
        typedef MeshParser::AttrNames AttrNames;
        OBJParser();
        virtual ~OBJParser() {}

        virtual bool parse(const std::string& filename);

        virtual size_t dim() const { return m_dim; }
        virtual size_t vertex_per_face() const { return m_vertex_per_face; }
        virtual size_t vertex_per_voxel() const { return 0; }; // Surface only.

        virtual size_t num_vertices() const {return m_vertices.size();}
        virtual size_t num_faces() const {return m_faces.size();}
        virtual size_t num_voxels() const {return m_voxels.size();}
        virtual size_t num_attributes() const;

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const;

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer);
        virtual void export_attribute(const std::string& name, Float* buffer);

    protected:
        void export_normals(Float* buffer) const;
        void export_textures(Float* buffer) const;
        void export_parameters(Float* buffer) const;
        bool parse_vertex_line(char* line);
        bool parse_vertex_coordinate(char* line);
        bool parse_vertex_normal(char* line);
        bool parse_vertex_texture(char* line);
        bool parse_vertex_parameter(char* line);
        bool parse_face_line(char* line);
        void unify_faces();
        void finalize_textures();
        void finalize_normals();
        void finalize_parameters();

        typedef std::list<VectorF> VertexList;
        typedef std::list<VectorI> FaceList;
        typedef std::list<VectorI>  VoxelList;
        typedef std::vector<VectorF> NormalVector;
        typedef std::vector<VectorF> TextureVector;
        typedef std::list<VectorF> ParameterList;

        VertexList m_vertices;
        FaceList   m_faces;
        FaceList   m_textures;
        FaceList   m_normals;
        FaceList   m_tris;
        FaceList   m_quads;
        FaceList   m_tri_textures;
        FaceList   m_tri_normals;
        FaceList   m_quad_textures;
        FaceList   m_quad_normals;
        VoxelList  m_voxels;
        NormalVector  m_corner_normals;
        TextureVector m_corner_textures;
        ParameterList m_parameters;
        size_t     m_dim;
        size_t     m_vertex_per_face;
        size_t     m_texture_dim;
        size_t     m_parameter_dim;
};

}
