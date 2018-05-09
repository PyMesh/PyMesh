/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include "GeogramBase.h"
#include <geogram/mesh/mesh.h>

#include <memory>

namespace PyMesh {
namespace GeogramMeshUtils {
    using GeoMesh = GEO::Mesh;
    using GeoMeshPtr = std::shared_ptr<GeoMesh>;

    GeoMeshPtr form_mesh(const Mesh::Ptr mesh);
    GeoMeshPtr form_mesh(const MatrixFr& vertices, const MatrixIr& faces);

}
}
