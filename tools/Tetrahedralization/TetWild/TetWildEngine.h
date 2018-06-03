/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <TetrahedralizationEngine.h>

namespace PyMesh {

class TetWildEngine : public TetrahedralizationEngine {
    public:
        virtual ~TetWildEngine() = default;

    public:
        virtual void run() override;
};

}
