/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Mesh.h>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Parameters/ParameterManager.h>

namespace PyMesh {

class WireTiler {
    public:
        typedef Mesh::Ptr MeshPtr;

        WireTiler(WireNetwork::Ptr unit_wire_network)
            : m_unit_wire_network(unit_wire_network) {}

        void with_parameters(ParameterManager::Ptr params) { m_params = params; }

    public:
        WireNetwork::Ptr tile_with_guide_bbox(
                const VectorF& bbox_min,
                const VectorF& bbox_max,
                const VectorI& repetitions);

        WireNetwork::Ptr tile_with_guide_mesh(const MeshPtr mesh);

        WireNetwork::Ptr tile_with_mixed_patterns(
                const std::vector<WireNetwork::Ptr>& patterns,
                const MeshPtr mesh,
                bool per_vertex_thickness,
                bool isotropic);

    private:
        WireNetwork::Ptr m_unit_wire_network;
        ParameterManager::Ptr m_params;
};

}
