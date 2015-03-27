/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Assembler/Materials/ElementWiseSymmetricMaterial.h>

#include "MaterialTest.h"

class ElementWiseSymmetricMaterialTest : public MaterialTest {
};

TEST_F(ElementWiseSymmetricMaterialTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    const size_t dim = mesh->get_dim();
    const size_t num_faces = mesh->get_num_faces();
    VectorF matrix_field(num_faces * 9);
    std::vector<MaterialPtr> materials(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        VectorF entries = VectorF::Ones(9) * (i+1);
        matrix_field.segment(9*i, 9) = entries;
        Eigen::Map<MatrixF> matrix(entries.data(), 3, 3);
        materials[i] = Material::create_symmetric(
                m_density, matrix);
    }
    mesh->add_attribute("face_centroid");
    mesh->add_attribute("symm_matrix");
    mesh->set_attribute("symm_matrix", matrix_field);

    MaterialPtr mat = MaterialPtr(new ElementWiseSymmetricMaterial(
                m_density, mesh, "symm_matrix"));

    VectorF face_centroids = mesh->get_attribute("face_centroid");
    for (size_t i=0; i<num_faces; i++) {
        VectorF centroid = face_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}

TEST_F(ElementWiseSymmetricMaterialTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = mesh->get_dim();
    const size_t num_voxels = mesh->get_num_voxels();
    VectorF matrix_field(num_voxels * 36);
    std::vector<MaterialPtr> materials(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        matrix_field.segment(i*36, 36) = VectorF::Ones(36) * (i+1);
        Eigen::Map<MatrixF> matrix(matrix_field.segment(i*36, 36).data(), 6, 6);
        materials[i] = Material::create_symmetric(
                m_density, matrix);
    }
    mesh->add_attribute("voxel_centroid");
    mesh->add_attribute("symm_matrix");
    mesh->set_attribute("symm_matrix", matrix_field);

    MaterialPtr mat = MaterialPtr(new ElementWiseSymmetricMaterial(
                m_density, mesh, "symm_matrix"));

    VectorF voxel_centroids = mesh->get_attribute("voxel_centroid");
    for (size_t i=0; i<num_voxels; i++) {
        VectorF centroid = voxel_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}
