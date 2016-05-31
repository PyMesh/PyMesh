/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <ConvexHullEngine.h>

#include <mutex>

namespace PyMesh {

class QhullEngine : public ConvexHullEngine {
    public:
        QhullEngine(): ConvexHullEngine() { }

    public:
        virtual void run(const MatrixFr& points);

    protected:
        void extract_hull(const MatrixFr& points);

    protected:
        std::mutex m_lock;
};

}
