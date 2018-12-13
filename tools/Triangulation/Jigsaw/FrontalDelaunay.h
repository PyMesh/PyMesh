/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_JIGSAW

#include <Triangulation/Triangulation.h>

namespace PyMesh {
namespace Jigsaw {
    class FrontalDelaunay : public Triangulation {
        public:
            FrontalDelaunay() = default;
            virtual ~FrontalDelaunay() = default;

        public:
            virtual void run();
    };
}
}

#endif
