/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once
#include <list>
#include <string>
#include <Core/EigenTypedef.h>
#include "Triangulation.h"

namespace PyMesh {

    class ShewchukTriangle : public Triangulation {
        public:
            ShewchukTriangle() = default;
            virtual ~ShewchukTriangle() = default;

        public:
            virtual void run();
    };
}
