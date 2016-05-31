/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>

namespace PyMesh {

class TetGenEngine : public TetrahedronizationEngine {
    public:
        virtual ~TetGenEngine() {}

    public:
        virtual void run();
};

}
