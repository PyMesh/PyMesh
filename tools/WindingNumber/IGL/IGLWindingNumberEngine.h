/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <WindingNumber/WindingNumberEngine.h>

namespace PyMesh {

class IGLWindingNumberEngine : public WindingNumberEngine {
    public:
        virtual ~IGLWindingNumberEngine() = default;

    public:
        virtual VectorF run(const MatrixFr& queries);
};

}

#endif
