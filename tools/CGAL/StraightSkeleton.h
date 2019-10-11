/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */

#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

class StraightSkeleton {
    public:
        void run(const MatrixFr& vertices, const MatrixIr& edges);

        const MatrixFr& get_skeleton_vertices() const {
            return m_skeleton_vertices;
        }
        const MatrixIr& get_skeleton_edges() const {
            return m_skeleton_edges;
        }

    private:
        MatrixFr m_skeleton_vertices;
        MatrixIr m_skeleton_edges;
};

}
