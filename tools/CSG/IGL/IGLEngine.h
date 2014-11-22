#pragma once

#include <CSG/CSGEngine.h>

class IGLEngine : public CSGEngine {
    public:
        virtual ~IGLEngine() {}

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();
};
