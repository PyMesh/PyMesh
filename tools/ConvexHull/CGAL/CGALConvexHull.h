#pragma once

#include <ConvexHullEngine.h>

class CGALConvexHull : public ConvexHullEngine {
    protected:
        void compute_index_map(const MatrixFr& points);
};
