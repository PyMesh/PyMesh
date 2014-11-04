#pragma once

#include <Mesh.h>
#include <Wires/WireNetwork/WireNetwork.h>

class WireTiler {
    public:
        typedef Mesh::Ptr MeshPtr;

        WireTiler(const WireNetwork& unit_wire_network)
            : m_unit_wire_network(unit_wire_network) {}

    public:
        WireNetwork tile_with_guide_bbox(
                const VectorF& bbox_min,
                const VectorF& bbox_max,
                const VectorI& repetitions);

        WireNetwork tile_with_guide_mesh(const MeshPtr mesh);

    private:
        const WireNetwork& m_unit_wire_network;
};
