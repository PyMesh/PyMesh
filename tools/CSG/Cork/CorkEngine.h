#pragma once

#include <CSG/CSGEngine.h>

class CorkEngine : public CSGEngine {
    public:
        virtual ~CorkEngine();

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();
};
