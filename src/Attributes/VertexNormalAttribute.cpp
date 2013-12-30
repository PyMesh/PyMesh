#include "VertexNormalAttribute.h"

#include <iostream>
#include <string>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

void VertexNormalAttribute::compute_from_mesh(Mesh& mesh) {
    compute_vertex_normals(mesh);
}

void VertexNormalAttribute::compute_vertex_normals(Mesh& mesh) {
    const size_t dim = mesh.get_dim();
    const size_t num_vertices = mesh.get_num_vertices();
    const size_t num_faces    = mesh.get_num_faces();
    const size_t vertex_per_face = mesh.get_vertex_per_face();

    const VectorF& normals = get_attribute(mesh, "face_normal");
    const VectorF& areas = get_attribute(mesh, "face_area");
    assert(normals.size() == 3 * num_faces);
    assert(areas.size() == num_faces);

    VectorF& v_normals = m_values;
    v_normals = VectorF::Zero(3 * num_vertices);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh.get_face(i);
        assert(face.size() == vertex_per_face);
        Vector3F face_normal = normals.segment<3>(i*3);
        Float face_area = areas[i];
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t vi = face[j];
            v_normals.segment<3>(vi*3) += face_normal * face_area;
        }
    }

    for (size_t i=0; i<num_vertices; i++) {
        Vector3F n = v_normals.segment<3>(3*i);
        n.normalize();
        v_normals.segment<3>(3*i) = n;
    }
}

const VectorF& VertexNormalAttribute::get_attribute(Mesh& mesh, const std::string& attr_name) {
    if (!mesh.has_attribute(attr_name)) {
        mesh.add_attribute(attr_name);
    }
    assert(mesh.has_attribute(attr_name));
    return mesh.get_attribute(attr_name);
}
