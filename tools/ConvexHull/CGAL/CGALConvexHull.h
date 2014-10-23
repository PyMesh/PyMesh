#pragma once

#include <ConvexHull/ConvexHullEngine.h>

class CGALConvexHull : public ConvexHullEngine {
    protected:
        void compute_index_map(const MatrixFr& points);
};
