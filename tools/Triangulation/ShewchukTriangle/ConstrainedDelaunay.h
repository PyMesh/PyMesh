/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE
#include <list>
#include <string>
#include <Core/EigenTypedef.h>
#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace Triangle {

    class ConstrainedDelaunay : public Triangulation {
        public:
            ConstrainedDelaunay() = default;
            virtual ~ConstrainedDelaunay() = default;

        public:
            virtual void run();
    };
}
}

#endif
