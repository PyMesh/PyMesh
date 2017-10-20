/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedralizationEngine.h>
#include <Geogram/GeogramBase.h>

namespace PyMesh {

class GeogramEngine : public TetrahedralizationEngine,
                      public GeogramBase {
    public:
        virtual ~GeogramEngine() = default;

    public:
        virtual void run() override;
};

}
