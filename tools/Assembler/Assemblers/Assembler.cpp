/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Assembler.h"

#include <sstream>

#include <Core/Exception.h>

#include "StiffnessAssembler.h"
#include "MassAssembler.h"
#include "LumpedMassAssembler.h"
#include "LaplacianAssembler.h"
#include "DisplacementStrainAssembler.h"
#include "ElasticityTensorAssembler.h"
#include "EngineerStrainStressAssembler.h"
#include "RigidMotionAssembler.h"

using namespace PyMesh;

Assembler::Ptr Assembler::create(const std::string& matrix_name) {
    if (matrix_name == "stiffness") {
        return Ptr(new StiffnessAssembler());
    } else if (matrix_name == "mass") {
        return Ptr(new MassAssembler());
    } else if (matrix_name == "lumped_mass") {
        return Ptr(new LumpedMassAssembler());
    } else if (matrix_name == "laplacian") {
        return Ptr(new LaplacianAssembler());
    } else if (matrix_name == "displacement_strain") {
        return Ptr(new DisplacementStrainAssembler());
    } else if (matrix_name == "elasticity_tensor") {
        return Ptr(new ElasticityTensorAssembler());
    } else if (matrix_name == "engineer_strain_stress") {
        return Ptr(new EngineerStrainStressAssembler());
    } else if (matrix_name == "rigid_motion") {
        return Ptr(new RigidMotionAssembler());
    } else {
        std::stringstream err_msg;
        err_msg << "Assembling " << matrix_name << " is not supported yet.";
        throw NotImplementedError(err_msg.str());
    }
}
