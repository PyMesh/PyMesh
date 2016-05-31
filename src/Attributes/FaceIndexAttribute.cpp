/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FaceIndexAttribute.h"

#include <Mesh.h>

using namespace PyMesh;

void FaceIndexAttribute::compute_from_mesh(Mesh& mesh) {
    const size_t num_faces = mesh.get_num_faces();
    VectorF& indices = m_values;
    indices = VectorF::LinSpaced(num_faces, 0, num_faces-1);
}
