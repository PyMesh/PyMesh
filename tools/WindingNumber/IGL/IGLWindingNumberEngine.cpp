/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IGLWindingNumberEngine.h"
#include <igl/winding_number.h>

using namespace PyMesh;

VectorF IGLWindingNumberEngine::run(const MatrixFr& queries) {
    VectorF winding_numbers;
    igl::winding_number(m_vertices, m_faces, queries, winding_numbers);
    return winding_numbers;
}
