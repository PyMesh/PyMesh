/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "CGALConvexHull.h"

namespace PyMesh {

class CGALConvexHull3D : public CGALConvexHull {
    public:
        virtual void run(const MatrixFr& points);
};

}
