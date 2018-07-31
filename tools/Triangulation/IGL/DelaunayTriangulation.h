/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <Triangulation/Triangulation.h>

namespace PyMesh {
    class DelaunayTriangulation : public Triangulation {
        public:
            DelaunayTriangulation() = default;
            virtual ~DelaunayTriangulation() = default;

        public:
            virtual void run();
    };
}

#endif
