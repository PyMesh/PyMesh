/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "Triangulation.h"

namespace PyMesh {
    class DelaunayTriangulation : public Triangulation {
        public:
            DelaunayTriangulation() = default;
            virtual ~DelaunayTriangulation() = default;

        public:
            virtual void run();
    };
}
