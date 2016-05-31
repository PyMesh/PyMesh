/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexNormalAttribute.h"

#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>

using namespace PyMesh;

void VertexNormalAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    if (dim == 3) {
        compute_vertex_normals_from_face(mesh);
    } else if (dim == 2) {
        compute_vertex_normals_from_edge(mesh);
    }
}

void VertexNormalAttribute::compute_vertex_normals_from_face(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces    = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    const VectorF& normals = get_attribute(mesh, "face_normal");
    const VectorF& areas = get_attribute(mesh, "face_area");
    assert(normals.size() == 3 * num_faces);
    assert(areas.size() == num_faces);

    VectorF& v_normals = m_values;
    v_normals = VectorF::Zero(dim * num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        assert(face.size() == vertex_per_face);
        VectorF face_normal = normals.segment(i*dim, dim);
        Float face_area = areas[i];
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t vi = face[j];
            v_normals.segment(vi*dim, dim) += face_normal * face_area;
        }
    }

    for (size_t i=0; i<num_vertices; i++) {
        VectorF n = v_normals.segment(dim*i, dim);
        Float n_len = n.norm();
        if (n_len > 0.0) n /= n_len;
        v_normals.segment(dim*i, dim) = n;
    }
}

void VertexNormalAttribute::compute_vertex_normals_from_edge(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    assert(dim == 2);
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    const VectorF& normals = get_attribute(mesh, "face_normal");

    VectorF& v_normals = m_values;
    v_normals = VectorF::Zero(dim * num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t prev = (j-1+vertex_per_face) % vertex_per_face;
            size_t next = (j+1) % vertex_per_face;
            Vector2F prev_edge = mesh.get_vertex(face[j]) -
                mesh.get_vertex(face[prev]);
            Vector2F next_edge = mesh.get_vertex(face[next]) -
                mesh.get_vertex(face[j]);

            Vector3F n = normals.segment(i*3, 3);
            Vector3F e1(prev_edge[0], prev_edge[1], 0);
            Vector3F e2(next_edge[0], next_edge[1], 0);
            Vector3F n1 = e1.cross(n);
            Vector3F n2 = e2.cross(n);

            v_normals.segment(face[j]*dim, dim) += (n1 + n2).segment(0, dim);
        }
    }

    for (size_t i=0; i<num_vertices; i++) {
        Float norm = v_normals.segment(i*dim, dim).norm();
        if (norm > 1e-6) {
            v_normals.segment(i*dim, dim) /= norm;
        }
    }
}

const VectorF& VertexNormalAttribute::get_attribute(Mesh& mesh, const std::string& attr_name) {
    if (!mesh.has_attribute(attr_name)) {
        mesh.add_attribute(attr_name);
    }
    assert(mesh.has_attribute(attr_name));
    return mesh.get_attribute(attr_name);
}
