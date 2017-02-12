/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>
#include <GeoGram/GeoGramBase.h>

namespace PyMesh {

class GeoGramEngine : public TetrahedronizationEngine,
                      public GeoGramBase {
    public:
        virtual ~GeoGramEngine() = default;

    public:
        virtual void run();
};

}
