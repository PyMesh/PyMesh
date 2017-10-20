/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TetrahedralizationEngine.h>

namespace PyMesh {

class TetGenEngine : public TetrahedralizationEngine {
    public:
        virtual ~TetGenEngine() =default;

    public:
        virtual void run() override;
};

}
