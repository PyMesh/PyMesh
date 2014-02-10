#include "Material.h"
#include "UniformMaterial.h"
#include "IsotropicMaterial.h"
#include "PeriodicMaterial.h"

Material::Ptr Material::create(Float density, const MatrixF& material_tensor) {
    return Ptr(new UniformMaterial(density, material_tensor));
}

Material::Ptr Material::create_isotropic(
        size_t dim, Float density, Float young, Float poisson) {
    if (dim == 2)
        return Ptr(new IsotropicMaterial<2>(density, young, poisson));
    else if (dim == 3)
        return Ptr(new IsotropicMaterial<3>(density, young, poisson));
}

Material::Ptr Material::create_periodic(
        Material::Ptr mat1, Material::Ptr mat2, VectorF axis, Float period) {
    return Ptr(new PeriodicMaterial(mat1, mat2, axis, period));
}
