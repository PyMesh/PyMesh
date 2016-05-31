/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "FEMeshFactory.h"
#include "TriangleMesh.h"
#include "TetrahedronMesh.h"

using namespace PyMesh;
using namespace FEMeshFactory;

FEMeshPtr FEMeshFactory::adapt(MeshPtr mesh) {
    const size_t dim = mesh->get_dim();
    if (dim == 2) {
        return FEMeshPtr(new TriangleMesh(mesh));
    } else {
        return FEMeshPtr(new TetrahedronMesh(mesh));
    }
}
