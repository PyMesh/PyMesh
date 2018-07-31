/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_TETGEN

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace PyMesh {

class TetGenEngine : public TetrahedralizationEngine {
    public:
        virtual ~TetGenEngine() =default;

    public:
        virtual void run() override;
};

}

#endif
