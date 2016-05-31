/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <SelfIntersectionResolver.h>

namespace PyMesh {

class IGLSelfIntersectionResolver : public SelfIntersectionResolver {
    public:
        virtual ~IGLSelfIntersectionResolver() {}

    public:
        virtual void run();
};

}
