#pragma once

#include <functional>
#include "PeriodicInflator.h"

class IsotropicPeriodicInflator : public PeriodicInflator {
    public:
        IsotropicPeriodicInflator(WireNetwork::Ptr wire_network);
        virtual ~IsotropicPeriodicInflator() {}

    protected:
        virtual void clip_to_center_cell();

    private:
        void initialize_center_cell_and_octa_cell();
        void clip_phantom_mesh_with_octa_cell();
        void remesh_boundary();
        void reflect();
        void update_face_sources();

    private:
        VectorF m_center_cell_bbox_min;
        VectorF m_center_cell_bbox_max;

        VectorF m_octa_cell_bbox_min;
        VectorF m_octa_cell_bbox_max;

        typedef std::function<VectorF(const MatrixFr& v)> SymmetryOperation;
        std::vector<SymmetryOperation> m_reflection_operators;
};
