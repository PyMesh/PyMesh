/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <ConvexHullEngine.h>

#include <mutex>

namespace PyMesh {

class TetGenConvexHullEngine : public ConvexHullEngine {
    public:
        TetGenConvexHullEngine(): ConvexHullEngine() {}

    public:
        virtual void run(const MatrixFr& points);
};

}
