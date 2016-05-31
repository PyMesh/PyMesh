/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireTiler.h"

#include <iostream>

#include "AABBTiler.h"
#include "MeshTiler.h"
#include "MixedMeshTiler.h"
#include "TilerEngine.h"

using namespace PyMesh;

WireNetwork::Ptr WireTiler::tile_with_guide_bbox(
        const VectorF& bbox_min,
        const VectorF& bbox_max,
        const VectorI& repetitions) {
    AABBTiler tiler(m_unit_wire_network, bbox_min, bbox_max, repetitions);
    if (m_params)
        tiler.with_parameters(m_params);
    return tiler.tile();
}

WireNetwork::Ptr WireTiler::tile_with_guide_mesh(const MeshPtr mesh) {
    MeshTiler tiler(m_unit_wire_network, mesh);
    if (m_params)
        tiler.with_parameters(m_params);
    return tiler.tile();
}

WireNetwork::Ptr WireTiler::tile_with_mixed_patterns(
        const std::vector<WireNetwork::Ptr>& patterns,
        const MeshPtr mesh,
        bool per_vertex_thickness,
        bool isotropic) {
    auto target_type = per_vertex_thickness ?
        ParameterCommon::VERTEX: ParameterCommon::EDGE;
    auto dof_type = isotropic ?
        ParameterManager::ISOTROPIC : ParameterManager::ORTHOTROPIC;
    MixedMeshTiler tiler(patterns, mesh, target_type, dof_type);
    if (m_params) {
        std::cerr << "Warning: user specified parameters will be ignored."
            << std::endl
            << "Mixed mesh tiler will extract parameters from mesh attributes"
            << std::endl;
    }
    return tiler.tile();
}
