#pragma once

#include <TetrahedronizationEngine.h>

class TetGenEngine : public TetrahedronizationEngine {
    public:
        virtual ~TetGenEngine() {}

    public:
        virtual void run();
};
