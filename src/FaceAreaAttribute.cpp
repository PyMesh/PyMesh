#include "FaceAreaAttribute.h"

#include <cassert>
#include <iostream>
#include "Mesh.h"
#include "Exception.h"

void FaceAreaAttribute::compute_from_mesh(Mesh& mesh) {
    size_t num_faces = mesh.get_num_faces();
    size_t num_vertex_per_face = mesh.get_vertex_per_face();

    VectorF& areas = m_values;
    areas = VectorF::Zero(num_faces);

    if (num_vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            areas[i] = compute_triangle_area(mesh, i);
        }
    } else {
        std::cerr << "Face with " << num_vertex_per_face << " is not supported yet." << std::endl;
        throw NotImplementedError("Only triangle faces are supported.");
        return;
    }
}

Float FaceAreaAttribute::compute_triangle_area(Mesh& mesh, size_t face_idx) {
    VectorI face = mesh.get_face(face_idx);
    assert(face.size() == 3);

    Vector3F v[3] = {
        mesh.get_vertex(face[0]),
        mesh.get_vertex(face[1]),
        mesh.get_vertex(face[2])
    };

    Float area = ((v[2] - v[0]).cross(v[1] - v[0])).norm() * 0.5;
    return area;
}
