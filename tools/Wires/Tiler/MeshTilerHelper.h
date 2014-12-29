#pragma once

#include <vector>

#include "TilerEngine.h"
#include <Mesh.h>
#include <Wires/Parameters/ParameterCommon.h>

namespace MeshTilerHelper {
    TilerEngine::FuncList get_2D_tiling_operators(Mesh::Ptr mesh);

    TilerEngine::FuncList get_3D_tiling_operators(Mesh::Ptr mesh);

    std::vector<ParameterCommon::Variables> extract_face_attributes(Mesh::Ptr mesh);

    std::vector<ParameterCommon::Variables> extract_voxel_attributes(Mesh::Ptr mesh);

    std::vector<ParameterCommon::Variables> extract_attributes(Mesh::Ptr mesh);
}
