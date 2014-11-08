#pragma once

#include "PeriodicInflator.h"

class PeriodicInflator3D : public PeriodicInflator {
    public:
        PeriodicInflator3D(WireNetwork& wire_network) :
            PeriodicInflator(wire_network) {}
        virtual ~PeriodicInflator3D() {}

    protected:
        virtual void clip_to_center_cell();
        void clip_phantom_mesh();
        void update_face_sources();
        void update_face_sources_old();
        void periodic_remesh();

    private:
        VectorF m_center_cell_bbox_min;
        VectorF m_center_cell_bbox_max;
};
