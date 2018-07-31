/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_TETWILD

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace PyMesh {

class TetWildEngine : public TetrahedralizationEngine {
    public:
        virtual ~TetWildEngine() = default;

    public:
        virtual void run() override;
};

}

#endif
