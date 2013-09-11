#include "VertexNormalAttribute.h"

#include <iostream>

#include "Mesh.h"
#include "EigenTypedef.h"

void VertexNormalAttribute::compute_from_mesh(Mesh& mesh) {
    compute_vertex_normals(mesh);
}

void VertexNormalAttribute::compute_vertex_normals(Mesh& mesh) {
    int dim = mesh.get_dim();
    size_t num_vertices = mesh.get_num_vertices();
    size_t num_faces    = mesh.get_num_faces();
    int vertex_per_face = mesh.get_vertex_per_face();
    int vertex_per_voxels = mesh.get_vertex_per_voxel();
    assert(vertex_per_face == 3);

    VectorF& normals = m_values;

    normals = VectorF::Zero(dim * num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);

        Vector3F v0 = mesh.get_vertex(face[0]);
        Vector3F v1 = mesh.get_vertex(face[1]);
        Vector3F v2 = mesh.get_vertex(face[2]);

        Vector3F n = (v1 - v0).cross(v2 - v0);

        normals.segment<3>(face[0]*3) += n;
        normals.segment<3>(face[1]*3) += n;
        normals.segment<3>(face[2]*3) += n;
    }

    for (size_t i=0; i<num_vertices; i++) {
        Vector3F n = normals.segment<3>(i*3);
        n.normalize();
        normals.segment<3>(i*3) = n;
    }
}
