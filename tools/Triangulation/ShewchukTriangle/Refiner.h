/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE
#include <list>
#include <string>
#include <Core/EigenTypedef.h>
#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace Triangle {

    class Refiner : public Triangulation {
        public:
            Refiner() = default;
            virtual ~Refiner() = default;

        public:
            virtual void refine(const MatrixFr& metric);
    };
}
}

#endif
