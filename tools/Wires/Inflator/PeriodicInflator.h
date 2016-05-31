/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Wires/Parameters/ParameterManager.h>

#include "InflatorEngine.h"

namespace PyMesh {

class AABBTree;

class PeriodicInflator : public InflatorEngine {
    public:
        PeriodicInflator(WireNetwork::Ptr wire_network) :
            InflatorEngine(wire_network) {
                m_parameter_manager =
                    ParameterManager::create_empty_manager(wire_network, 0.5);
                m_with_shape_velocities = false;
            }
        virtual ~PeriodicInflator() {}

    public:
        virtual void with_shape_velocities() { m_with_shape_velocities = true; }
        virtual void inflate();
        virtual const std::vector<MatrixFr>& get_shape_velocities() const {
            return m_shape_velocities;
        }
        virtual void set_uniform_thickness(Float thickness);
        virtual void set_thickness(const VectorF& thickness);

        void set_parameter(ParameterManager::Ptr manager);

    protected:
        void generate_phantom_mesh();
        void initialize_AABB_tree();
        virtual void clip_to_center_cell()=0;
        void refine_phantom_mesh();
        void get_center_cell_bbox(VectorF& bbox_min, VectorF& bbox_max);
        void update_shape_velocities();

    protected:
        bool m_with_shape_velocities;
        WireNetwork::Ptr m_phantom_wires;
        MatrixFr m_phantom_vertices;
        MatrixIr m_phantom_faces;
        VectorI  m_phantom_face_sources;
        ParameterManager::Ptr m_parameter_manager;
        std::vector<MatrixFr> m_shape_velocities;
        std::shared_ptr<AABBTree> m_tree;
};

}
