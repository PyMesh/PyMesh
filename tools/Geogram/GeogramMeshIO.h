/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>
#include "GeogramBase.h"
#include <geogram/mesh/mesh.h>

namespace PyMesh {
namespace GeogramMeshIO {

GeoMeshPtr load_mesh(const std::string& filename);
void save_mesh(const std::string& filename, GeoMeshPtr mesh);

}
}
