/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_MMG

#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace MMG {
    class Delaunay : public Triangulation {
        public:
            Delaunay() = default;
            virtual ~Delaunay() = default;

        public:
            virtual void run();
            virtual void refine(const MatrixFr& metric);
    };
}
}

#endif
