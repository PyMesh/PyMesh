#pragma once

#include "InflatorEngine.h"

class PeriodicInflator : public InflatorEngine {
    public:
        PeriodicInflator(WireNetwork& wire_network) :
            InflatorEngine(wire_network) {}
        virtual ~PeriodicInflator() {}

    public:
        virtual void inflate();

    protected:
        void initialize_phantom_wires();
        void inflate_phantom_wires();
        void update_phantom_periodic_face_sources(const VectorI& face_sources);
        virtual void clip_to_center_cell()=0;
        void get_center_cell_bbox(VectorF& bbox_min, VectorF& bbox_max);

    protected:
        WireNetwork m_phantom_wires;
        MatrixFr m_phantom_vertices;
        MatrixIr m_phantom_faces;
        VectorI  m_phantom_face_sources;
};
