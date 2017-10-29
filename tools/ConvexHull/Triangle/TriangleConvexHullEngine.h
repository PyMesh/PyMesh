/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <ConvexHullEngine.h>

#include <mutex>

namespace PyMesh {

class TriangleConvexHullEngine : public ConvexHullEngine {
    public:
        TriangleConvexHullEngine(): ConvexHullEngine() { }

    public:
        virtual void run(const MatrixFr& points);

    protected:
        void extract_hull(const MatrixFr& points);

    protected:
        std::mutex m_lock;
};

}
