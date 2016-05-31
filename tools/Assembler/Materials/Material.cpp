/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Material.h"

#include <sstream>
#include <Core/Exception.h>

#include "ElementWiseIsotropicMaterial.h"
#include "ElementWiseOrthotropicMaterial.h"
#include "ElementWiseSymmetricMaterial.h"
#include "IsotropicMaterial.h"
#include "OrthotropicMaterial.h"
#include "PeriodicMaterial.h"
#include "SymmetricMaterial.h"
#include "UniformMaterial.h"

using namespace PyMesh;

Material::Ptr Material::create(Float density, const MatrixF& material_tensor) {
    return Ptr(new UniformMaterial(density, material_tensor));
}

Material::Ptr Material::create_isotropic(
        size_t dim, Float density, Float young, Float poisson) {
    if (dim == 2)
        return Ptr(new IsotropicMaterial<2>(density, young, poisson));
    else if (dim == 3)
        return Ptr(new IsotropicMaterial<3>(density, young, poisson));
    else {
        std::stringstream err_msg;
        err_msg << "Unsupported dimension: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

Material::Ptr Material::create_symmetric(Float density, const MatrixF& material_matrix) {
    return Ptr(new SymmetricMaterial(density, material_matrix));
}

Material::Ptr Material::create_orthotropic(Float density, 
        const VectorF& young_modulus,
        const VectorF& poisson_ratio,
        const VectorF& shear_modulus) {
    const size_t dim = young_modulus.size();
    if (dim == 2) {
        return Ptr(new OrthotropicMaterial<2>(
                    density, young_modulus, poisson_ratio, shear_modulus));
    } else if (dim == 3) {
        return Ptr(new OrthotropicMaterial<3>(
                    density, young_modulus, poisson_ratio, shear_modulus));
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dimension: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

Material::Ptr Material::create_periodic(
        Material::Ptr mat1, Material::Ptr mat2, VectorF axis,
        Float period, Float ratio, Float phase) {
    return Ptr(new PeriodicMaterial(mat1, mat2, axis, period, ratio, phase));
}

Material::Ptr Material::create_element_wise_isotropic(Float density, Mesh::Ptr mesh,
        const std::string& young_field_name,
        const std::string& poisson_field_name) {
    return Ptr(new ElementWiseIsotropicMaterial(density, mesh,
                young_field_name, poisson_field_name));
}

Material::Ptr Material::create_element_wise_orthotropic(Float density, Mesh::Ptr mesh,
        const std::vector<std::string>& young_field_names,
        const std::vector<std::string>& poisson_field_names,
        const std::vector<std::string>& shear_field_names) {
    return Ptr(new ElementWiseOrthotropicMaterial(density, mesh,
                young_field_names, poisson_field_names, shear_field_names));
}

Material::Ptr Material::create_element_wise_symmetric(Float density, Mesh::Ptr mesh,
        const std::string& matrix_field_name) {
    return Ptr(new ElementWiseSymmetricMaterial(density, mesh,
                matrix_field_name));
}
