#pragma once

#include "CGALConvexHull.h"

class CGALConvexHull2D : public CGALConvexHull {
    public:
        virtual void run(const MatrixFr& points);
};
