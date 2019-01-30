/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <list>
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class SVGParser {
    public:
        SVGParser() = default;

        void parse(const std::string& filename, bool keep_bbox=true);

        size_t get_dim() const { return 2; }
        size_t get_num_vertices() const { return m_vertices.size(); }
        size_t get_num_edges() const { return m_edges.size(); }

        void export_vertices(Float* buffer) const;
        void export_edges(int* buffer) const;

    protected:
        void add_bbox(Float width, Float height);
        void add_bezier_curve(
                const Vector2F& p0,
                const Vector2F& p1,
                const Vector2F& p2,
                const Vector2F& p3,
                Float tol, int level, bool start_with_previous);
        void add_line_segment(
                const Vector2F& p0,
                const Vector2F& v1, bool start_with_previous);

    private:
        using VertexList = std::list<Vector2F>;
        using EdgeList = std::list<Vector2I>;

        VertexList m_vertices;
        EdgeList m_edges;
};

}
