/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_QUARTET

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace PyMesh {

class QuartetEngine : public TetrahedralizationEngine {
    public:
        virtual ~QuartetEngine() = default;

    public:
        virtual void run() override;
};

}

#endif
