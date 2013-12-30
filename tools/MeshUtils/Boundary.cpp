#include "Boundary.h"

#include <Core/Exception.h>
#include <Mesh.h>

#include "BoundaryEdges.h"
#include "BoundaryFaces.h"

Boundary::Ptr Boundary::extract_surface_boundary(const Mesh& mesh) {
    return Ptr(new BoundaryEdges(mesh));
}

Boundary::Ptr Boundary::extract_volume_boundary(const Mesh& mesh) {
    if (mesh.get_dim() == 2) {
        throw RuntimeError("2D mesh has no volume boundary!");
    }
    if (mesh.get_num_voxels() == 0) {
        throw RuntimeError("Mesh contains 0 voxels");
    }
    return Ptr(new BoundaryFaces(mesh));
}
