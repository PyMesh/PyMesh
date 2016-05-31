/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ElementWiseOrthotropicMaterial.h"

#include <cassert>
#include <sstream>

#include <Core/Exception.h>

#include "Material.h"

using namespace PyMesh;

ElementWiseOrthotropicMaterial::ElementWiseOrthotropicMaterial(
        Float density, MeshPtr material_mesh,
        const ElementWiseOrthotropicMaterial::FieldNames& young_field_names,
        const ElementWiseOrthotropicMaterial::FieldNames& poisson_field_names,
        const ElementWiseOrthotropicMaterial::FieldNames& shear_field_names):
    ElementWiseMaterial(density, material_mesh),
    m_young_field_names(young_field_names),
    m_poisson_field_names(poisson_field_names),
    m_shear_field_names(shear_field_names) {
        const size_t dim = material_mesh->get_dim();
        if (dim == 2) {
            if (young_field_names.size() != 2) {
                throw RuntimeError("Expect 2 young field names");
            }
            if (poisson_field_names.size() != 2) {
                throw RuntimeError("Expect 2 poisson field names");
            }
            if (shear_field_names.size() != 1) {
                throw RuntimeError("Expect 1 shear field name");
            }
        } else if (dim == 3) {
            if (young_field_names.size() != 3) {
                throw RuntimeError("Expect 3 young field names");
            }
            if (poisson_field_names.size() != 6) {
                throw RuntimeError("Expect 6 poisson field names");
            }
            if (shear_field_names.size() != 3) {
                throw RuntimeError("Expect 3 shear field name");
            }
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
        }
        update();
}

void ElementWiseOrthotropicMaterial::update() {
    const size_t dim = m_material_mesh->get_dim();
    if (dim == 2) {
        update_2D();
    } else if (dim == 3) {
        update_3D();
    } else {
        std::stringstream err_msg;
        err_msg << "Unknow dimention: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

void ElementWiseOrthotropicMaterial::update_2D() {
    const size_t num_faces = m_material_mesh->get_num_faces();
    const VectorF& young_x = m_material_mesh->get_attribute(m_young_field_names[0]);
    const VectorF& young_y = m_material_mesh->get_attribute(m_young_field_names[1]);
    const VectorF& poisson_xy = m_material_mesh->get_attribute(m_poisson_field_names[0]);
    const VectorF& poisson_yx = m_material_mesh->get_attribute(m_poisson_field_names[1]);
    const VectorF& shear = m_material_mesh->get_attribute(m_shear_field_names[0]);

    assert(young_x.size() == num_faces);
    assert(young_y.size() == num_faces);
    assert(poisson_xy.size() == num_faces);
    assert(poisson_yx.size() == num_faces);
    assert(shear.size() == num_faces);

    m_materials.resize(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        Vector2F elem_young(young_x[i], young_y[i]);
        Vector2F elem_poisson(poisson_xy[i], poisson_yx[i]);
        VectorF elem_shear(1);
        elem_shear[0] = shear[i];

        m_materials[i] = Material::create_orthotropic(m_density,
                elem_young, elem_poisson, elem_shear);
    }
}

void ElementWiseOrthotropicMaterial::update_3D() {
    const size_t num_voxels = m_material_mesh->get_num_voxels();
    const VectorF& young_x = m_material_mesh->get_attribute(m_young_field_names[0]);
    const VectorF& young_y = m_material_mesh->get_attribute(m_young_field_names[1]);
    const VectorF& young_z = m_material_mesh->get_attribute(m_young_field_names[2]);
    const VectorF& poisson_yz = m_material_mesh->get_attribute(m_poisson_field_names[0]);
    const VectorF& poisson_zy = m_material_mesh->get_attribute(m_poisson_field_names[1]);
    const VectorF& poisson_zx = m_material_mesh->get_attribute(m_poisson_field_names[2]);
    const VectorF& poisson_xz = m_material_mesh->get_attribute(m_poisson_field_names[3]);
    const VectorF& poisson_xy = m_material_mesh->get_attribute(m_poisson_field_names[4]);
    const VectorF& poisson_yx = m_material_mesh->get_attribute(m_poisson_field_names[5]);
    const VectorF& shear_yz = m_material_mesh->get_attribute(m_shear_field_names[0]);
    const VectorF& shear_zx = m_material_mesh->get_attribute(m_shear_field_names[1]);
    const VectorF& shear_xy = m_material_mesh->get_attribute(m_shear_field_names[2]);

    assert(young_x.size() == num_voxels);
    assert(young_y.size() == num_voxels);
    assert(young_z.size() == num_voxels);
    assert(poisson_yz.size() == num_voxels);
    assert(poisson_zy.size() == num_voxels);
    assert(poisson_zx.size() == num_voxels);
    assert(poisson_xz.size() == num_voxels);
    assert(poisson_xy.size() == num_voxels);
    assert(poisson_yx.size() == num_voxels);
    assert(shear_yz.size() == num_voxels);
    assert(shear_zx.size() == num_voxels);
    assert(shear_xy.size() == num_voxels);

    m_materials.resize(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        Vector3F elem_young(young_x[i], young_y[i], young_z[i]);
        VectorF elem_poisson(6);
        elem_poisson << poisson_yz[i], poisson_zy[i],
                        poisson_zx[i], poisson_xz[i],
                        poisson_xy[i], poisson_yx[i];
        Vector3F elem_shear(shear_yz[i], shear_zx[i], shear_xy[i]);

        m_materials[i] = Material::create_orthotropic(m_density,
                elem_young, elem_poisson, elem_shear);
    }
}

