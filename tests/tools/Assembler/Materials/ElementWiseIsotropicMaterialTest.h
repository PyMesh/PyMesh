#pragma once

#include <string>
#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Environment.h>
#include <Mesh.h>
#include <MeshFactory.h>

#include <Assembler/Materials/ElementWiseIsotropicMaterial.h>
#include <Assembler/Materials/IsotropicMaterial.h>
#include <Assembler/Materials/Material.h>

class ElementWiseIsotropicMaterialTest : public ::testing::Test {
    protected:
        typedef Material::Ptr MaterialPtr;
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
            m_density = 1.0;
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshFactory()
                    .load_file(mesh_file)
                    .create_shared();
        }

        void assert_material_eq(MaterialPtr mat1, MaterialPtr mat2, size_t dim, const VectorF& coord) {
            for (size_t i=0; i<dim; i++) {
                for (size_t j=0; j<dim; j++) {
                    for (size_t k=0; k<dim; k++) {
                        for (size_t l=0; l<dim; l++) {
                            ASSERT_FLOAT_EQ(
                                    mat1->get_material_tensor(i,j,k,l,coord),
                                    mat2->get_material_tensor(i,j,k,l,coord));
                        }
                    }
                }
            }
        }

    protected:
        std::string m_data_dir;
        Float m_density;
};

TEST_F(ElementWiseIsotropicMaterialTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    const size_t dim = mesh->get_dim();
    const size_t num_faces = mesh->get_num_faces();
    VectorF young(num_faces);
    VectorF poisson(num_faces);
    std::vector<MaterialPtr> materials(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        young[i] = 2.0 * i;
        poisson[i] = 0.01 * i;
        materials[i] = Material::create_isotropic(
                dim, m_density, young[i], poisson[i]);
    }
    mesh->add_attribute("face_centroid");
    mesh->add_attribute("young");
    mesh->set_attribute("young", young);
    mesh->add_attribute("poisson");
    mesh->set_attribute("poisson", poisson);

    MaterialPtr mat = MaterialPtr(new ElementWiseIsotropicMaterial(
                m_density, mesh, "young", "poisson"));

    VectorF face_centroids = mesh->get_attribute("face_centroid");
    for (size_t i=0; i<num_faces; i++) {
        VectorF centroid = face_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}

TEST_F(ElementWiseIsotropicMaterialTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    const size_t dim = mesh->get_dim();
    const size_t num_voxels = mesh->get_num_voxels();
    VectorF young(num_voxels);
    VectorF poisson(num_voxels);
    std::vector<MaterialPtr> materials(num_voxels);
    for (size_t i=0; i<num_voxels; i++) {
        young[i] = 2.0 * i;
        poisson[i] = 0.01 * i;
        materials[i] = Material::create_isotropic(
                dim, m_density, young[i], poisson[i]);
    }
    mesh->add_attribute("voxel_centroid");
    mesh->add_attribute("young");
    mesh->set_attribute("young", young);
    mesh->add_attribute("poisson");
    mesh->set_attribute("poisson", poisson);

    MaterialPtr mat = MaterialPtr(new ElementWiseIsotropicMaterial(
                m_density, mesh, "young", "poisson"));

    VectorF voxel_centroids = mesh->get_attribute("voxel_centroid");
    for (size_t i=0; i<num_voxels; i++) {
        VectorF centroid = voxel_centroids.segment(i*dim, dim);
        MaterialPtr curr_mat = materials[i];
        assert_material_eq(mat, curr_mat, dim, centroid);
    }
}

