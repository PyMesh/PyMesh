/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "Triangulation.h"

namespace PyMesh {
    class LexicographicTriangulation : public Triangulation {
        public:
            LexicographicTriangulation() = default;
            virtual ~LexicographicTriangulation() = default;

        public:
            virtual void run();
    };
}
