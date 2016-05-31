/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class BoundaryRemesher {
    public:
        BoundaryRemesher(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        typedef std::function<bool(const VectorF&)> IndicatorFunc;
        void remesh(IndicatorFunc bd_indicator, Float max_area);

        const MatrixFr& get_vertices() const { return m_vertices; }
        const MatrixIr& get_faces() const { return m_faces; }

    private:
        void extract_bd_faces(IndicatorFunc f);
        void remesh_bd_faces(Float max_area);
        void merge_bd_and_non_bd_faces();
        void remove_duplicated_vertices();

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;

        MatrixFr m_bd_vertices;
        MatrixIr m_bd_faces;

        MatrixFr m_non_bd_vertices;
        MatrixIr m_non_bd_faces;
};

}
