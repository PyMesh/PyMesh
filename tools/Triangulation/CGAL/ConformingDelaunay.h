/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace _CGAL {
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
