/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_TRIANGLE
#include <list>
#include <string>
#include <Core/EigenTypedef.h>
#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace Triangle {

    class ConformingDelaunay : public Triangulation {
        public:
            ConformingDelaunay() = default;
            virtual ~ConformingDelaunay() = default;

        public:
            virtual void run();
    };
}
}

#endif
