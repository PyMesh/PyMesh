/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <list>
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class WireParser {
    public:
        WireParser();

        void parse(const std::string& filename);

        size_t get_dim() const { return m_dim; }
        size_t get_num_vertices() const { return m_vertices.size(); }
        size_t get_num_edges() const { return m_edges.size(); }

        void export_vertices(Float* buffer) const;
        void export_edges(int* buffer) const;

    private:
        void reset();
        bool parse_vertex_line(char* line);
        bool parse_edge_line(char* line);

    private:
        typedef std::list<VectorF> VertexList;
        typedef std::list<VectorI> EdgeList;

        size_t m_dim;
        VertexList m_vertices;
        EdgeList   m_edges;
};

}
