/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <list>

#include <Core/EigenTypedef.h>
#include "MeshParser.h"

namespace PyMesh {
class NodeParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        NodeParser();
        virtual ~NodeParser() = default;

    public:
        virtual bool parse(const std::string& filename);

        virtual size_t dim() const { return m_dim; }
        virtual size_t vertex_per_face() const { return m_vertex_per_face; }
        virtual size_t vertex_per_voxel() const { return m_vertex_per_voxel; }

        virtual size_t num_vertices() const {return m_num_vertices;}
        virtual size_t num_faces() const {return m_num_faces; }
        virtual size_t num_voxels() const {return m_num_voxels;}
        virtual size_t num_attributes() const;

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const;

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer);
        virtual void export_attribute(const std::string& name, Float* buffer);

    protected:
        typedef std::list<VectorF> VertexList;
        typedef std::list<VectorI> FaceList;
        typedef std::list<VectorI> VoxelList;
        typedef std::list<VectorF> AttributeList;
        typedef std::list<int>     BoundaryNodeMarkers;
        typedef std::list<int>     BoundaryFaceMarkers;
        typedef std::list<int>     RegionAttributes;

        bool parse_nodes(const std::string& filename);
        bool parse_faces(const std::string& filename);
        bool parse_elements(const std::string& filename);
        void extract_faces_from_voxels();

    protected:
        VertexList    m_vertices;
        FaceList      m_faces;
        VoxelList     m_voxels;
        AttributeList m_node_attributes;
        size_t        m_num_vertices;
        size_t        m_num_faces;
        size_t        m_num_voxels;
        size_t        m_dim;
        size_t        m_vertex_per_face;
        size_t        m_vertex_per_voxel;
        size_t        m_num_node_attributes;

        BoundaryNodeMarkers m_boundary_node_markers;
        BoundaryFaceMarkers m_boundary_face_markers;
        RegionAttributes    m_region_attributes;
};
}
