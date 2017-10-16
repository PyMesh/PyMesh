/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>

namespace PyMesh {

class CGALMeshGen : public TetrahedronizationEngine {
    public:
        virtual ~CGALMeshGen() = default;

    public:
        virtual void run() override;
};

}
