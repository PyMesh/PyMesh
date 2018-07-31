/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <ConvexHull/ConvexHullEngine.h>

namespace PyMesh {

class CGALConvexHull : public ConvexHullEngine {
    protected:
        void compute_index_map(const MatrixFr& points);
};

}

#endif
