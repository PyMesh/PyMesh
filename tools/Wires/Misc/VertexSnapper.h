/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

namespace VertexSnapper {
    void snap_vertices_to_border(MatrixFr& vertices, MatrixIr& faces,
            const VectorF& bbox_min, const VectorF& bbox_max, Float tol);
}

}
