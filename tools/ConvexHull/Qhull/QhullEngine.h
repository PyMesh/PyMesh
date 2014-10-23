#pragma once

#include <ConvexHull/ConvexHullEngine.h>

#include <mutex>

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
