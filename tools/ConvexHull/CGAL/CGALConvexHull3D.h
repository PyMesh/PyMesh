#pragma once

#include "CGALConvexHull.h"

class CGALConvexHull3D : public CGALConvexHull {
    public:
        virtual void run(const MatrixFr& points);
};
