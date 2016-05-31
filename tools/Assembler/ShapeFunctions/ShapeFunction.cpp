/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ShapeFunction.h"

#include <sstream>

#include <Core/Exception.h>

#include "LinearShapeFunctionOverTriangle.h"
#include "LinearShapeFunctionOverTetrahedron.h"

using namespace PyMesh;

ShapeFunction::Ptr ShapeFunction::create(
        ShapeFunction::FEMeshPtr mesh, const std::string& func_type) {
    const size_t dim = mesh->getDim();
    const size_t nodes_per_element = mesh->getNodePerElement();
    if (func_type == "linear") {
        if (dim == 2 && nodes_per_element == 3) {
            return Ptr(new LinearShapeFunctionOverTriangle(mesh));
        } else if (dim == 3 && nodes_per_element == 4) {
            return Ptr(new LinearShapeFunctionOverTetrahedron(mesh));
        } else {
            std::stringstream err_msg;
            err_msg << "Linear shape function is incompatible with " << nodes_per_element
                << " nodes per element in " << dim << "D.";
            throw RuntimeError(err_msg.str());
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Shape function type \"" << func_type
            << "\" is not supported yet.";
        throw NotImplementedError(err_msg.str());
    }
}
