/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

class MeshCleaner {
    public:
        void clean(MatrixFr& vertices, MatrixIr& faces, Float tol);

        VectorI compute_importance_level(const MatrixFr& vertices);
        void remove_duplicated_vertices(MatrixFr& vertices, MatrixIr& faces, Float tol);
        VectorI remove_short_edges(MatrixFr& vertices, MatrixIr& faces, Float tol);
        void remove_isolated_vertices(MatrixFr& vertices, MatrixIr& faces);
        void remove_obtuse_triangle(MatrixFr& vertices, MatrixIr& faces);
        void remove_fin_faces(MatrixFr& vertices, MatrixIr& faces);
};

}
