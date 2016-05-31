/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexSnapper.h"

#include <cmath>

#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>
#include "BoxChecker.h"

using namespace PyMesh;

void VertexSnapper::snap_vertices_to_border(
        MatrixFr& vertices, MatrixIr& faces,
        const VectorF& bbox_min, const VectorF& bbox_max, Float tol) {
    const size_t num_faces = faces.rows();
    const size_t vertex_per_face = faces.cols();
    Float sq_tol = tol * tol;

    BoxChecker bbox_checker(bbox_min, bbox_max);

    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t v0_idx = f[j];
            size_t v1_idx = f[(j+1)%vertex_per_face];

            const VectorF& v0 = vertices.row(v0_idx);
            const VectorF& v1 = vertices.row(v1_idx);
            Float sq_len = (v1 - v0).squaredNorm();
            if (sq_len < sq_tol) {
                if ( bbox_checker.is_on_boundary(v0) &&
                    !bbox_checker.is_on_boundary(v1)) {
                    vertices.row(v1_idx) = v0.transpose();
                } else if ( bbox_checker.is_on_boundary(v1) &&
                           !bbox_checker.is_on_boundary(v0)) {
                    vertices.row(v0_idx) = v1.transpose();
                }
            }
        }
    }

    bool updated = true;

    while (updated) {
        ObtuseTriangleRemoval slim_tri_remover(vertices, faces);
        size_t num_slim_tri_removed = slim_tri_remover.run(M_PI * 0.99);

        ShortEdgeRemoval edge_remover(
                slim_tri_remover.get_vertices(),
                slim_tri_remover.get_faces());
        edge_remover.run(1e-12);

        vertices = edge_remover.get_vertices();
        faces = edge_remover.get_faces();

        updated = (num_slim_tri_removed > 0);
    }
}

