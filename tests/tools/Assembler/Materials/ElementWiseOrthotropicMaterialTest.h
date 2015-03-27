/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/ElementWiseOrthotropicMaterial.h>
#include <Assembler/Materials/Material.h>

#include "MaterialTest.h"

class ElementWiseOrthotropicMaterialTest : public MaterialTest {
};

TEST_F(ElementWiseOrthotropicMaterialTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    const size_t dim = mesh->get_dim();
    const size_t num_faces = mesh->get_num_faces();
    VectorF young_x(num_faces);
    VectorF young_y(num_faces);
    VectorF poisson_xy(num_faces);
    VectorF poisson_yx(num_faces);
    VectorF shear(num_faces);

    std::vector<MaterialPtr> materials(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        young_x[i] = young_y[i] = 2.0 + i;
        poisson_xy[i] = poisson_yx[i] = 0.01 * i;
        shear[i] = 1.0 + i * 0.5;

        VectorF elem_young(2);
        VectorF elem_poisson(2);
        VectorF elem_shear(1);

        elem_young << young_x[i], young_y[i];
        elem_poisson << poisson_xy[i], poisson_yx[i];
        elem_shear << shear[i];

        materials[i] = Material::create_orthotropic(m_density,
                elem_young, elem_poisson, elem_shear);
    }

    mesh->add_attribute("face_centroid");
    mesh->add_attribute("young_x");
    mesh->set_attribute("young_x", young_x);
    mesh->add_attribute("young_y");
    mesh->set_attribute("young_y", young_y);
    mesh->add_attribute("poisson_xy");
    mesh->set_attribute("poisson_xy", poisson_xy);
    mesh->add_attribute("poisson_yx");
    mesh->set_attribute("poisson_yx", poisson_yx);
    mesh->add_attribute("shear");
    mesh->set_attribute("shear", shear);

    std::vector<std::string> young_fields;
    std::vector<std::string> poisson_fields;
    std::vector<std::string> shear_fields;

    young_fields.push_back("young_x");
    young_fields.push_back("young_y");
    poisson_fields.push_back("poisson_xy");
    poisson_fields.push_back("poisson_yx");
    shear_fields.push_back("shear");

    MaterialPtr mat = MaterialPtr(new ElementWiseOrthotropicMaterial(
                m_density, mesh, young_fields, poisson_fields, shear_fields));

    VectorF face_centroids = mesh->get_attribute("face_centroid");
    for (size_t i=0; i<num_faces; i++) {
        VectorF centroid = face_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}

TEST_F(ElementWiseOrthotropicMaterialTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = mesh->get_dim();
    const size_t num_voxels = mesh->get_num_voxels();
    VectorF young_x(num_voxels);
    VectorF young_y(num_voxels);
    VectorF young_z(num_voxels);
    VectorF poisson_yz(num_voxels);
    VectorF poisson_zy(num_voxels);
    VectorF poisson_zx(num_voxels);
    VectorF poisson_xz(num_voxels);
    VectorF poisson_xy(num_voxels);
    VectorF poisson_yx(num_voxels);
    VectorF shear_yz(num_voxels);
    VectorF shear_zx(num_voxels);
    VectorF shear_xy(num_voxels);

    std::vector<MaterialPtr> materials(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        young_x[i] = young_y[i] = young_z[i] = 2.0 + i;
        poisson_yz[i] = poisson_zy[i] =
            poisson_xz[i] = poisson_zx[i] =
            poisson_xy[i] = poisson_yx[i] = 0.01 * i;
        shear_yz[i] = shear_zx[i] = shear_xy[i] = 1.0 + i * 0.5;

        VectorF elem_young(3);
        VectorF elem_poisson(6);
        VectorF elem_shear(3);

        elem_young << young_x[i], young_y[i], young_z[i];
        elem_poisson << poisson_yz[i], poisson_zy[i],
                        poisson_zx[i], poisson_xz[i],
                        poisson_xy[i], poisson_yz[i];
        elem_shear << shear_yz[i], shear_zx[i], shear_xy[i];
        materials[i] = Material::create_orthotropic(m_density,
                elem_young, elem_poisson, elem_shear);
    }

    mesh->add_attribute("voxel_centroid");
    mesh->add_attribute("young_x");
    mesh->set_attribute("young_x", young_x);
    mesh->add_attribute("young_y");
    mesh->set_attribute("young_y", young_y);
    mesh->add_attribute("young_z");
    mesh->set_attribute("young_z", young_z);
    mesh->add_attribute("poisson_yz");
    mesh->set_attribute("poisson_yz", poisson_yz);
    mesh->add_attribute("poisson_zy");
    mesh->set_attribute("poisson_zy", poisson_zy);
    mesh->add_attribute("poisson_zx");
    mesh->set_attribute("poisson_zx", poisson_zx);
    mesh->add_attribute("poisson_xz");
    mesh->set_attribute("poisson_xz", poisson_xz);
    mesh->add_attribute("poisson_xy");
    mesh->set_attribute("poisson_xy", poisson_xy);
    mesh->add_attribute("poisson_yx");
    mesh->set_attribute("poisson_yx", poisson_yx);
    mesh->add_attribute("shear_yz");
    mesh->set_attribute("shear_yz", shear_yz);
    mesh->add_attribute("shear_zx");
    mesh->set_attribute("shear_zx", shear_zx);
    mesh->add_attribute("shear_xy");
    mesh->set_attribute("shear_xy", shear_xy);

    std::vector<std::string> young_fields;
    std::vector<std::string> poisson_fields;
    std::vector<std::string> shear_fields;

    young_fields.push_back("young_x");
    young_fields.push_back("young_y");
    young_fields.push_back("young_z");
    poisson_fields.push_back("poisson_yz");
    poisson_fields.push_back("poisson_zy");
    poisson_fields.push_back("poisson_zx");
    poisson_fields.push_back("poisson_xz");
    poisson_fields.push_back("poisson_xy");
    poisson_fields.push_back("poisson_yx");
    shear_fields.push_back("shear_yz");
    shear_fields.push_back("shear_zx");
    shear_fields.push_back("shear_xy");

    MaterialPtr mat = MaterialPtr(new ElementWiseOrthotropicMaterial(m_density, mesh,
                young_fields, poisson_fields, shear_fields));

    VectorF voxel_centroids = mesh->get_attribute("voxel_centroid");
    for (size_t i=0; i<num_voxels; i++) {
        VectorF centroid = voxel_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}
