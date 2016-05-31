/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Integrator.h"

#include <Core/Exception.h>

#include "LinearTriangleIntegrator.h"
#include "LinearTetrahedronIntegrator.h"

using namespace PyMesh;

Integrator::Ptr Integrator::create(Integrator::FEMeshPtr mesh,
        Integrator::ShapeFuncPtr shape_func) {
    const size_t dim = mesh->getDim();
    const size_t nodes_per_element = mesh->getNodePerElement();
    if (dim == 2 && nodes_per_element == 3) {
        return Ptr(new LinearTriangleIntegrator(mesh, shape_func));
    } else if (dim == 3 && nodes_per_element == 4) {
        return Ptr(new LinearTetrahedronIntegrator(mesh, shape_func));
    } else {
        throw NotImplementedError("No integrator support this mesh type.");
    }
}
