/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

namespace PyMesh {
namespace MeshUtils {
    MatrixI extract_exterior_faces(const MatrixI& voxels);
}
}
