/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Mesh.h>

#include "FEMeshAdaptor.h"

namespace PyMesh {
namespace FEMeshFactory {
    typedef Mesh::Ptr MeshPtr;
    typedef FEMeshAdaptor::Ptr FEMeshPtr;
    FEMeshPtr adapt(MeshPtr mesh);
}
}
