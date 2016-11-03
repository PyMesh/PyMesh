/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>

namespace PyMesh {

class GeoGramEngine : public TetrahedronizationEngine {
    public:
        virtual ~GeoGramEngine() = default;

    public:
        virtual void run();
};

}
