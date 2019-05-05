/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>

#include <vector>

namespace PyMesh {
namespace VoxelUtils {
    /**
     * Return orientation of each tet.
     *
     *   * + value means tet is positiviely oriented.
     *   * 0 value means tet is degenerate (i.e. collapsed into a plane or line or point)
     *   * - value means tet is inverted.
     */
    VectorF get_tet_orientations(
            const MatrixFr& vertices, 
            const MatrixIr& tets);

    VectorF is_delaunay(
            const MatrixFr& vertices,
            const MatrixIr& tets);
}
}
