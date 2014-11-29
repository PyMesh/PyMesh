#pragma once

#include <Core/EigenTypedef.h>

namespace VertexSnapper {
    void snap_vertices_to_border(MatrixFr& vertices, MatrixIr& faces,
            const VectorF& bbox_min, const VectorF& bbox_max, Float tol);
}
