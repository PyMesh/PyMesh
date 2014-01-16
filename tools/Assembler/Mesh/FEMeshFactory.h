#pragma once

#include <Mesh.h>

#include "FEMeshAdaptor.h"

namespace FEMeshFactory {
    typedef Mesh::Ptr MeshPtr;
    typedef FEMeshAdaptor::Ptr FEMeshPtr;
    FEMeshPtr adapt(MeshPtr mesh);
}
