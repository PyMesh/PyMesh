#pragma once

#include <Core/EigenTypedef.h>

#include <SkeletonExtraction/SkeletonEngine.h>

namespace PyMesh {

class CreateSkeleton2 : public SkeletonEngine
{
    virtual void run(const MatrixFr& points);
};

} // end of namespace PyMesh