#pragma once

#include <TetrahedronizationEngine.h>

class CGALMeshGen : public TetrahedronizationEngine {
    public:
        virtual ~CGALMeshGen() {}

    public:
        virtual void run();
};
