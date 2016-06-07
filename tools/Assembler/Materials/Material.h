/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Mesh.h>

namespace PyMesh {

class Material {
    public:
        typedef std::shared_ptr<Material> Ptr;
        static Ptr create(Float density, const MatrixF& material_tensor);
        static Ptr create_isotropic(size_t dim, Float desnity, Float young, Float poisson);
        static Ptr create_symmetric(Float density, const MatrixF& material_matrix);
        static Ptr create_orthotropic(Float density,
                const VectorF& young_modulus,
                const VectorF& poisson_ratio,
                const VectorF& shear_modulus);
        static Ptr create_periodic(Ptr mat1, Ptr mat2, VectorF axis,
                Float period, Float ratio, Float phase);
        static Ptr create_element_wise_isotropic(Float density, Mesh::Ptr mesh,
                const std::string& young_field_name,
                const std::string& poisson_field_name);
        static Ptr create_element_wise_orthotropic(Float density, Mesh::Ptr mesh,
                const std::vector<std::string>& young_field_names,
                const std::vector<std::string>& poisson_field_names,
                const std::vector<std::string>& shear_field_names);
        static Ptr create_element_wise_symmetric(Float density, Mesh::Ptr mesh,
                const std::string& matrix_field_nmae);

    public:
        virtual Float get_material_tensor(size_t i, size_t j, size_t k, size_t l, VectorF coord) const=0;
        virtual MatrixF strain_to_stress(const MatrixF& strain, VectorF coord) const=0;
        virtual Float get_density(VectorF coord) const=0;
        virtual Float get_density() const=0;
        virtual size_t get_dim() const=0;

        virtual void update() {throw NotImplementedError("updata() is not implemented by subclasses."); }
};

}
