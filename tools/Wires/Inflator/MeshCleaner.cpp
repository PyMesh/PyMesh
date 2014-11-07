#include "MeshCleaner.h"

#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>

VectorI MeshCleaner::clean(MatrixFr& vertices, MatrixIr& faces, Float tol) {
    remove_duplicated_vertices(vertices, faces, tol);
    VectorI face_sources = remove_short_edges(vertices, faces, tol);
    remove_isolated_vertices(vertices, faces);
    return face_sources;
}

void MeshCleaner::remove_duplicated_vertices(
        MatrixFr& vertices, MatrixIr& faces, Float tol) {
    DuplicatedVertexRemoval remover(vertices, faces);
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
