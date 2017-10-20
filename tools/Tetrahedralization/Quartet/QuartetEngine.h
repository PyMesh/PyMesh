/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <TetrahedralizationEngine.h>

namespace PyMesh {

class QuartetEngine : public TetrahedralizationEngine {
    public:
        virtual ~QuartetEngine() = default;

    public:
        virtual void run() override;
};

}
