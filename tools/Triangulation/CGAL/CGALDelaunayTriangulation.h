/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "Triangulation.h"

namespace PyMesh {
    class CGALDelaunayTriangulation : public Triangulation {
        public:
            CGALDelaunayTriangulation() = default;
            virtual ~CGALDelaunayTriangulation() = default;

        public:
            virtual void run();
    };
}
