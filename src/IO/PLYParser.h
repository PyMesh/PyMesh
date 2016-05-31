/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "MeshParser.h"
#include <list>
#include <map>
#include <string>
#include <vector>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class PLYParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        virtual ~PLYParser() {}

        virtual bool parse(const std::string& filename);

        virtual size_t dim() const { return m_dim; }
        virtual size_t vertex_per_face() const { return m_vertex_per_face; }
        virtual size_t vertex_per_voxel() const { return m_vertex_per_voxel; }

        virtual size_t num_vertices() const;
        virtual size_t num_faces() const;
        virtual size_t num_voxels() const;
        virtual size_t num_attributes() const;

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const;

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer);
        virtual void export_attribute(const std::string& name, Float* buffer);

    public:
        void add_property(const std::string& elem_name,
                const std::string& prop_name, size_t size);
        void add_property_value(const std::string& elem_name,
                const std::string& prop_name, Float value);

        void init_vertices();
        void init_faces();
        void init_voxels();

    protected:
        typedef VectorF VertexArray;
        typedef VectorI FaceArray;
        typedef VectorI VoxelArray;
        typedef std::map<std::string, std::vector<Float> > AttributeMap;

        VertexArray   m_vertices;
        FaceArray     m_faces;
        VoxelArray    m_voxels;
        AttributeMap m_attributes;
        size_t       m_dim;
        size_t       m_vertex_per_face;
        size_t       m_vertex_per_voxel;
        size_t       m_num_vertices;
        size_t       m_num_faces;
        size_t       m_num_voxels;
};

}
