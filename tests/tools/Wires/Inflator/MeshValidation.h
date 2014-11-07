#pragma once

#include <Core/EigenTypedef.h>

namespace MeshValidation {
    bool is_water_tight(const MatrixFr& vertices, const MatrixIr& faces);
    bool is_manifold(const MatrixFr& vertices, const MatrixIr& faces);
    bool is_periodic(const MatrixFr& vertices, const MatrixIr&faces);
}
