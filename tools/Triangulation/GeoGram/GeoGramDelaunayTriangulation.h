/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include "Triangulation.h"
#include <GeoGram/GeoGramBase.h>

namespace PyMesh {
    class GeoGramDelaunayTriangulation : public Triangulation,
                                         public GeoGramBase {
        public:
            GeoGramDelaunayTriangulation() = default;
            virtual ~GeoGramDelaunayTriangulation() = default;

        public:
            virtual void run();
    };
}
