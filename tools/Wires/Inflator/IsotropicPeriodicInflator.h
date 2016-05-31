/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <functional>
#include "PeriodicInflator.h"

namespace PyMesh {

class IsotropicPeriodicInflator : public PeriodicInflator {
    public:
        IsotropicPeriodicInflator(WireNetwork::Ptr wire_network);
        virtual ~IsotropicPeriodicInflator() {}

    protected:
        virtual void clip_to_center_cell();

    private:
        void initialize_center_cell_and_octa_cell();
        void clip_phantom_mesh_with_octa_cell();
        void snap_to_cell_border();
        void clean_up_clipped_mesh();
        void refine_long_clip_box_edges();
        void remesh_boundary();
        void reflect_old();
        void reflect();
        void merge_boundary();
        void ensure_periodicity();
        void update_face_sources();
        Float get_distance_threshold() const;

    private:
        VectorF m_center_cell_bbox_min;
        VectorF m_center_cell_bbox_max;

        VectorF m_octa_cell_bbox_min;
        VectorF m_octa_cell_bbox_max;

        typedef std::function<VectorF(const MatrixFr& v)> SymmetryOperation;
        std::vector<SymmetryOperation> m_reflection_operators;
};

}
