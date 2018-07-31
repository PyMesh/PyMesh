/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_GEOGRAM

#include <Core/Exception.h>
#include "GeogramMeshIO.h"
#include <geogram/mesh/mesh_io.h>

using namespace PyMesh;

GeoMeshPtr GeogramMeshIO::load_mesh(const std::string& filename) {
    constexpr size_t dim = 3;
    auto geo_mesh = std::make_shared<GeoMesh>(dim, false);
    const bool r = GEO::mesh_load(filename, *geo_mesh);
    if (!r) {
        throw IOError("Failed to load geogram mesh from " + filename);
    }
    return geo_mesh;
}

void GeogramMeshIO::save_mesh(const std::string& filename, GeoMeshPtr geo_mesh) {
    const bool r = GEO::mesh_save(*geo_mesh, filename);
    if (!r) {
        throw IOError("Failed to save geogram mesh to " + filename);
    }
}

#endif
