/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace _CGAL {
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
