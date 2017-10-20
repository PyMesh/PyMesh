/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>
#include <Geogram/GeogramBase.h>

namespace PyMesh {

class GeogramEngine : public TetrahedronizationEngine,
                      public GeogramBase {
    public:
        virtual ~GeogramEngine() = default;

    public:
        virtual void run() override;
};

}
