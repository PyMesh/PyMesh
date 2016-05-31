/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <ConvexHullEngine.h>

namespace PyMesh {

class CGALConvexHull : public ConvexHullEngine {
    protected:
        void compute_index_map(const MatrixFr& points);
};

}
