/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace PyMesh {

class MMGEngine : public TetrahedralizationEngine {
    public:
        virtual ~MMGEngine() = default;

    public:
        virtual void run() override;
};

}
