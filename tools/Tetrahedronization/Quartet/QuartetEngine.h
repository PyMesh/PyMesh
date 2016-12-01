/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>

namespace PyMesh {

class QuartetEngine : public TetrahedronizationEngine {
    public:
        virtual ~QuartetEngine() = default;

    public:
        virtual void run();
};

}
