#pragma once

#include <Core/EigenTypedef.h>

class MeshCleaner {
    public:
        VectorI clean(MatrixFr& vertices, MatrixIr& faces, Float tol);
        void remove_duplicated_vertices(MatrixFr& vertices, MatrixIr& faces, Float tol);
        VectorI remove_short_edges(MatrixFr& vertices, MatrixIr& faces, Float tol);
        void remove_isolated_vertices(MatrixFr& vertices, MatrixIr& faces);
};
