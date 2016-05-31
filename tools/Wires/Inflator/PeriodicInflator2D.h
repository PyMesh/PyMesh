/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PeriodicInflator.h"
#include <list>

namespace PyMesh {

class PeriodicInflator2D : public PeriodicInflator {
    public:
        PeriodicInflator2D(WireNetwork::Ptr wire_network) :
            PeriodicInflator(wire_network) {}
        virtual ~PeriodicInflator2D() {}

    protected:
        virtual void clip_to_center_cell();

        void clip_phantom_mesh(std::list<Float>& vertices,
                std::list<size_t>& faces, std::list<int>& face_sources);
};

}
