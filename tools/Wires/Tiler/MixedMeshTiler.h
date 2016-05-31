/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include "TilerEngine.h"
#include <Mesh.h>
#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ParameterManager.h>

namespace PyMesh {

class MixedMeshTiler : public TilerEngine {
    public:
        typedef Mesh::Ptr MeshPtr;
        typedef std::vector<WireNetwork::Ptr> Wires;
        typedef ParameterManager::TargetType TargetType;
        typedef ParameterManager::DofType DofType;
        typedef ParameterManager::Variables Variables;

        MixedMeshTiler(const Wires& unit_wires, MeshPtr mesh,
                TargetType thickness_type = ParameterCommon::VERTEX,
                DofType dof_type = ParameterManager::ISOTROPIC);
        virtual ~MixedMeshTiler() {}

    public:
        virtual WireNetwork::Ptr tile();

    private:
        size_t get_num_cells() const;
        FuncList get_tiling_operators() const;

        void set_active_wire_network(size_t i);
        void scale_to_unit_box();
        void append_vertices(const Func& f);
        void append_edges(size_t v_count);
        void append_thicknesses(const Variables& vars);
        void append_offsets(const Variables& vars, const Func& f);

    private:
        MeshPtr m_mesh;
        Wires m_unit_wires;
        TargetType m_target_type;
        DofType m_dof_type;

        std::vector<MatrixFr> m_tiled_vertices;
        std::vector<MatrixIr> m_tiled_edges;
        std::vector<MatrixFr> m_tiled_thicknesses;
        std::vector<MatrixFr> m_tiled_offsets;
};

}
