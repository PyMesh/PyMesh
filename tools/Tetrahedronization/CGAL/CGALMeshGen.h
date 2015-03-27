/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TetrahedronizationEngine.h>

class CGALMeshGen : public TetrahedronizationEngine {
    public:
        virtual ~CGALMeshGen() {}

    public:
        virtual void run();
};
