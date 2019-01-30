/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE

#include <ConvexHull/ConvexHullEngine.h>

#include <mutex>

namespace PyMesh {

class TriangleConvexHullEngine : public ConvexHullEngine {
    public:
        TriangleConvexHullEngine(): ConvexHullEngine() { }

    public:
        virtual void run(const MatrixFr& points);
};

}

#endif
