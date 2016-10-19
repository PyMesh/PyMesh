/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "Triangulation.h"

namespace PyMesh {
    class GeoGramDelaunayTriangulation : public Triangulation {
        public:
            GeoGramDelaunayTriangulation() = default;
            virtual ~GeoGramDelaunayTriangulation() = default;

        public:
            virtual void run();

        private:
            static bool GEOGRAM_INITIALIZED;
    };
}
