/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "PeriodicInflator.h"

namespace PyMesh {

class PeriodicInflator3D : public PeriodicInflator {
    public:
        PeriodicInflator3D(WireNetwork::Ptr wire_network) :
            PeriodicInflator(wire_network) {}
        virtual ~PeriodicInflator3D() {}

    protected:
        virtual void clip_to_center_cell();
        void clip_phantom_mesh();
        void update_face_sources();
        void periodic_remesh();

    private:
        VectorF m_center_cell_bbox_min;
        VectorF m_center_cell_bbox_max;
};

}
