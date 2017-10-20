/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#include <list>

#include <Core/EigenTypedef.h>
#include "MeshParser.h"

namespace PyMesh {

class VEGAParser : public MeshParser {
    public:
        using AttrNames = MeshParser::AttrNames;

        VEGAParser();
        virtual ~VEGAParser() = default;

    public:
        virtual bool parse(const std::string& filename) override;

        virtual size_t dim() const override { return m_dim; }
        virtual size_t vertex_per_face() const override {
            return m_vertex_per_face;
        }
        virtual size_t vertex_per_voxel() const override {
            return m_vertex_per_voxel;
        }

        virtual size_t num_vertices() const override {return m_num_vertices;}
        virtual size_t num_faces() const override {return m_num_faces; }
        virtual size_t num_voxels() const override {return m_num_voxels;}
        virtual size_t num_attributes() const override { return 0; }

        virtual AttrNames get_attribute_names() const override;
        virtual size_t get_attribute_size(
                const std::string& name) const override;

        virtual void export_vertices(Float* buffer) override;
        virtual void export_faces(int* buffer) override;
        virtual void export_voxels(int* buffer) override;
        virtual void export_attribute(
                const std::string& name, Float* buffer) override;

    protected:
        using VertexList          = std::list<VectorF>;
        using FaceList            = std::list<VectorI>;
        using VoxelList           = std::list<VectorI>;
        using AttributeList       = std::list<VectorF>;

        void parse_vertices(std::ifstream& fin);
        void parse_elements(std::ifstream& fin);
        void parse_material(std::ifstream& fin);
        void parse_region(std::ifstream& fin);
        void parse_include(std::ifstream& fin);
        void parse_set(std::ifstream& fin);
        void extract_faces_from_voxels();

    protected:
        size_t        m_num_vertices;
        size_t        m_num_faces;
        size_t        m_num_voxels;
        size_t        m_dim;
        size_t        m_vertex_per_face;
        size_t        m_vertex_per_voxel;
        size_t        m_num_node_attributes;
        bool          m_index_start_from_zero;

        VertexList m_vertices;
        FaceList m_faces;
        VoxelList m_voxels;
};

}
