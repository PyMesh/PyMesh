/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "VoxelUtils.h"

#include <Core/Exception.h>
#include <Predicates/predicates.h>

using namespace PyMesh;

VectorF VoxelUtils::get_tet_orientations(
        const MatrixFr& vertices,
        const MatrixIr& voxels) {
    const size_t dim = vertices.cols();
    const size_t vertex_per_voxel = voxels.cols();
    const size_t num_tets = voxels.rows();
    if (dim != 3) {
        throw RuntimeError("Degenerate tets is only defined for 3D meshes.");
    }
    if (vertex_per_voxel != 4) {
        throw RuntimeError("Degenerate tet expect a tet mesh.");
    }

    exactinit();
    VectorF results(num_tets);
    results.setZero();
    for (size_t i=0; i<num_tets; i++) {
        const Vector4I tet = voxels.row(i);
        Float a[3]{
            vertices(tet[0], 0),
            vertices(tet[0], 1),
            vertices(tet[0], 2)};
        Float b[3]{
            vertices(tet[1], 0),
            vertices(tet[1], 1),
            vertices(tet[1], 2)};
        Float c[3]{
            vertices(tet[2], 0),
            vertices(tet[2], 1),
            vertices(tet[2], 2)};
        Float d[3]{
            vertices(tet[3], 0),
            vertices(tet[3], 1),
            vertices(tet[3], 2)};

        // NOTE:
        //
        // orient3d(a,b,c,d) returns positive if d is below the plane defined by
        // triangle (a, b, c).  The tet vertex ordering used by MSH format is
        // the opposite (i.e. d is above the triangle (a,b,c)), so this check
        // switch the ordering of a and b to return positive if tet is not
        // inverted.
        results[i] = orient3d(b, a, c, d);
    }
    return results;
}

