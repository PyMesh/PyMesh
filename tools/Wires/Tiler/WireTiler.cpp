#include "WireTiler.h"

#include "AABBTiler.h"
#include "MeshTiler.h"
#include "TilerEngine.h"

WireNetwork::Ptr WireTiler::tile_with_guide_bbox(
        const VectorF& bbox_min,
        const VectorF& bbox_max,
        const VectorI& repetitions) {
    AABBTiler tiler(m_unit_wire_network, bbox_min, bbox_max, repetitions);
    tiler.with_parameters(m_params);
    return tiler.tile();
}

WireNetwork::Ptr WireTiler::tile_with_guide_mesh(const MeshPtr mesh) {
    MeshTiler tiler(m_unit_wire_network, mesh);
    tiler.with_parameters(m_params);
    return tiler.tile();
}

