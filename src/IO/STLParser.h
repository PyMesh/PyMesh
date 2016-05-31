/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshParser.h"
#include <list>
#include <string>
#include <fstream>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class STLParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        virtual ~STLParser() {}

        virtual bool parse(const std::string& filename);

        // Only extract surface in obj file.
        virtual size_t vertex_per_voxel() const { return 0; };

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
        bool attribute_exists(const std::string& name) const;
        bool is_binary(const std::string& filename);
        bool parse_ascii(const std::string& filename);
        bool parse_ascii_facet(std::ifstream& fin);
        bool parse_ascii_normal(char* line);
        bool parse_ascii_vertex(char* line);

        bool parse_binary(const std::string& filename);

        void merge_identical_vertices();
        void validate_normals();
        bool has_normal() const;
        Float compute_bbox_diagonal_length() const;

        typedef std::vector<Vector3F> VertexList;
        typedef std::list<Vector3I> FaceList;
        typedef std::list<VectorI>  VoxelList;
        typedef std::list<Vector3F> NormalList;

        VertexList m_vertices;
        FaceList   m_faces;
        VoxelList  m_voxels;
        NormalList m_facet_normals;
};

}
