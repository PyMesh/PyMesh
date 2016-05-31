/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <list>
#include <Core/EigenTypedef.h>

#include "MeshParser.h"

namespace PyMesh {

class OFFParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        OFFParser();
        virtual ~OFFParser() {}

        virtual bool parse(const std::string& filename);

        virtual size_t dim() const { return m_dim; }
        virtual size_t vertex_per_face() const { return m_vertex_per_face; }
        virtual size_t vertex_per_voxel() const { return 0; }; // Surface only.

        virtual size_t num_vertices() const {return m_vertices.size();}
        virtual size_t num_faces() const {return m_faces.size();}
        virtual size_t num_voxels() const {return 0;} // Surface only.
        virtual size_t num_attributes() const;

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const;

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer) {}
        virtual void export_attribute(const std::string& name, Float* buffer);

    protected:
        typedef std::list<VectorF> VertexList;
        typedef std::list<VectorI> FaceList;
        typedef std::list<VectorF> ColorList;

        void check_header(char* line);
        void parse_geometry_counts(char* line);
        void parse_vertex_line(char* line);
        void parse_face_line(char* line);
        void unify_faces();
        void finalize_colors();
        void export_color(const ColorList& colors, const std::string& name, Float* buffer);

    protected:
        VertexList m_vertices;
        FaceList   m_faces;
        FaceList   m_tris;
        FaceList   m_quads;
        ColorList  m_vertex_colors;
        ColorList  m_face_colors;
        size_t     m_num_vertices;
        size_t     m_num_faces;
        size_t     m_dim;
        size_t     m_vertex_per_face;
};

}
