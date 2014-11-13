#pragma once

#include <Mesh.h>
#include <Wires/WireNetwork/WireNetwork.h>

class WireTiler {
    public:
        typedef Mesh::Ptr MeshPtr;

        WireTiler(WireNetwork::Ptr unit_wire_network)
            : m_unit_wire_network(unit_wire_network) {}

    public:
        WireNetwork::Ptr tile_with_guide_bbox(
                const VectorF& bbox_min,
                const VectorF& bbox_max,
                const VectorI& repetitions);

        WireNetwork::Ptr tile_with_guide_mesh(const MeshPtr mesh);

    private:
        WireNetwork::Ptr m_unit_wire_network;
};
