#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

class CreateSkeleton2
{
public:
    CreateSkeleton2(const MatrixFr& vertices);
    MatrixFr Skeleton();
protected:
    MatrixFr Compute_skeleton();
protected:
    MatrixFr m_vertices;
};

} // end of namespace PyMesh