/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

using namespace PyMesh;
namespace MeshValidation {
    bool is_water_tight(const MatrixFr& vertices, const MatrixIr& faces);
    bool is_manifold(const MatrixFr& vertices, const MatrixIr& faces);
    bool is_periodic(const MatrixFr& vertices, const MatrixIr&faces);
    bool face_source_is_valid(const MatrixFr& vertices, const MatrixIr& faces, const VectorI& face_sources);
}
