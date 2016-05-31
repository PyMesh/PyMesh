/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WindingNumberEngine.h>

namespace PyMesh {

class IGLWindingNumberEngine : public WindingNumberEngine {
    public:
        virtual ~IGLWindingNumberEngine() {}

    public:
        virtual VectorF run(const MatrixFr& queries);
};

}
