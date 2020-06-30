/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#if defined(WITH_MMG) && defined(WITH_TETGEN) && defined(WITH_IGL)

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace PyMesh {

class MMGEngine : public TetrahedralizationEngine {
    public:
        virtual ~MMGEngine() = default;

    public:
        virtual void run() override;
};

}

#endif
