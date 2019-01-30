/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include "GeogramBase.h"
#include <geogram/mesh/mesh.h>

#include <memory>

namespace PyMesh {
namespace GeogramMeshUtils {

GeoMeshPtr mesh_to_geomesh(const Mesh::Ptr mesh);
GeoMeshPtr raw_to_geomesh(const MatrixFr& vertices, const MatrixIr& faces);
GeoMeshPtr wire_network_to_geomesh(const MatrixFr& vertices, const Matrix2Ir& edges);
Mesh::Ptr geomesh_to_mesh(const GeoMeshPtr geo_mesh);

}
}

#endif
