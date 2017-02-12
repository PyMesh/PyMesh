/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "MeshParser.h"

#include <list>
#include <vector>
#include <string>
#include <fstream>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class POLYParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        POLYParser() : m_vertex_per_face(3), m_vertex_per_voxel(4) {}
        virtual ~POLYParser() = default;

        virtual bool parse(const std::string& filename);

        virtual size_t dim() const { return 3; }
        virtual size_t vertex_per_face() const { return m_vertex_per_face; }
        virtual size_t vertex_per_voxel() const { return m_vertex_per_voxel; }

        virtual size_t num_vertices() const { return m_vertices.size() / 3; }
        virtual size_t num_faces() const {
            return m_faces.size() / m_vertex_per_face;
        }
        virtual size_t num_voxels() const {
            return m_voxels.size() / m_vertex_per_voxel;
        }
        virtual size_t num_attributes() const { return 0; }

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const {
            return 0;
        }

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer);
        virtual void export_attribute(const std::string& name, Float* buffer);

    protected:
        void parse_vertices(std::ifstream& fin);
        void parse_faces(std::ifstream& fin);

    protected:
        std::vector<Float>  m_vertices;
        std::list<size_t> m_faces;
        std::list<size_t> m_voxels;
        size_t m_vertex_per_face;
        size_t m_vertex_per_voxel;
};

}
