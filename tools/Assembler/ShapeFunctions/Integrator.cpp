#include "Integrator.h"

#include <Core/Exception.h>

#include "LinearTriangleIntegrator.h"
#include "LinearTetrahedronIntegrator.h"

Integrator::Ptr Integrator::create(Integrator::FEMeshPtr mesh) {
    const size_t dim = mesh->getDim();
    const size_t nodes_per_element = mesh->getNodePerElement();
    if (dim == 2 && nodes_per_element == 3) {
        ShapeFuncPtr shape_func = ShapeFunction::create(mesh, "linear");
        return Ptr(new LinearTriangleIntegrator(mesh, shape_func));
    } else if (dim == 3 && nodes_per_element == 4) {
        ShapeFuncPtr shape_func = ShapeFunction::create(mesh, "linear");
        return Ptr(new LinearTetrahedronIntegrator(mesh, shape_func));
    } else {
        throw NotImplementedError("No integrator support this mesh type.");
    }
}
