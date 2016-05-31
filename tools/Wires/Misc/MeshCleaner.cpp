/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshCleaner.h"

#include <cmath>

#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/FinFaceRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>
#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <Misc/Timer.h>

#include "BoxChecker.h"

using namespace PyMesh;

void MeshCleaner::clean(MatrixFr& vertices, MatrixIr& faces, Float tol) {
    remove_isolated_vertices(vertices, faces);
    remove_duplicated_vertices(vertices, faces, tol);
    remove_fin_faces(vertices, faces);
    VectorI face_sources = remove_short_edges(vertices, faces, tol);
    remove_obtuse_triangle(vertices, faces);
    remove_isolated_vertices(vertices, faces);
    //return face_sources;
}

VectorI MeshCleaner::compute_importance_level(const MatrixFr& vertices) {
    VectorF bbox_min = vertices.colwise().minCoeff();
    VectorF bbox_max = vertices.colwise().maxCoeff();
    BoxChecker checker(bbox_min, bbox_max);

    const size_t num_vertices = vertices.rows();
    VectorI level = VectorI::Zero(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = vertices.row(i);
        if (checker.is_on_boundary_corners(v)) {
            level[i] = 3;
        } else if (checker.is_on_boundary_edges(v)) {
            level[i] = 2;
        } else if (checker.is_on_boundary(v)) {
            level[i] = 1;
        }
    }

    return level;
}

void MeshCleaner::remove_duplicated_vertices(
        MatrixFr& vertices, MatrixIr& faces, Float tol) {
    VectorI importance_level = compute_importance_level(vertices);
    DuplicatedVertexRemoval remover(vertices, faces);
    remover.set_importance_level(importance_level);
    remover.run(tol);
    vertices = remover.get_vertices();
    faces = remover.get_faces();
}

VectorI MeshCleaner::remove_short_edges(
        MatrixFr& vertices, MatrixIr& faces, Float tol) {
    ShortEdgeRemoval remover(vertices, faces);
    remover.run(tol);

    vertices = remover.get_vertices();
    faces = remover.get_faces();

    return remover.get_face_indices();
}

void MeshCleaner::remove_isolated_vertices(
        MatrixFr& vertices, MatrixIr& faces) {
    IsolatedVertexRemoval remover(vertices, faces);
    remover.run();
    vertices = remover.get_vertices();
    faces = remover.get_faces();
}

void MeshCleaner::remove_obtuse_triangle(MatrixFr& vertices, MatrixIr& faces) {
    ObtuseTriangleRemoval remover(vertices, faces);
    remover.run(M_PI - 1e-3);
    vertices = remover.get_vertices();
    faces = remover.get_faces();
}

void MeshCleaner::remove_fin_faces(MatrixFr& vertices, MatrixIr& faces) {
    FinFaceRemoval remover(vertices, faces);
    size_t num_face_removed = remover.run();
    if (num_face_removed > 0) {
        faces = remover.get_faces();
    }
}

