#pragma once

#include "PeriodicInflator.h"
#include <list>

class PeriodicInflator2D : public PeriodicInflator {
    public:
        PeriodicInflator2D(WireNetwork& wire_network) :
            PeriodicInflator(wire_network) {}
        virtual ~PeriodicInflator2D() {}

    protected:
        virtual void clip_to_center_cell();

        void clip_phantom_mesh(std::list<Float>& vertices,
                std::list<size_t>& faces, std::list<int>& face_sources);
};
