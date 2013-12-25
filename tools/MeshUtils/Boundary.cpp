#include "Boundary.h"

#include <Mesh.h>

#include "BoundaryEdges.h"
#include "BoundaryFaces.h"

Boundary::BoundaryPtr Boundary::extract_boundary(const Mesh& mesh) {
    const size_t num_voxels = mesh.get_num_voxels();
    if (num_voxels == 0) {
        return BoundaryPtr(new BoundaryEdges(mesh));
    } else {
        return BoundaryPtr(new BoundaryFaces(mesh));
    }
}
