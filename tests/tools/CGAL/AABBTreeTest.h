/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <CGAL/AABBTree.h>

#include <Mesh.h>
#include <MeshFactory.h>

class AABBTreeTest : public TestBase {
    //protected:
    //    typedef Mesh::Ptr MeshPtr;

    //    virtual void SetUp() {
    //        TestBase::SetUp();
    //        std::string project_dir = Environment::get("PYMESH_PATH");
    //        m_mesh_data_dir = project_dir + "/tests/data/";
    //    }

    //    MeshPtr load_mesh(const std::string& mesh_file) {
    //        MeshFactory factory;
    //        factory.load_file(m_mesh_data_dir + mesh_file);
    //        return factory.create_shared();
    //    }

    //protected:
    //    std::string m_mesh_data_dir;
};

TEST_F(AABBTreeTest, cube) {
    MeshPtr mesh = load_mesh("cube.obj");
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t dim = mesh->get_dim();

    mesh->add_attribute("face_centroid");
    VectorF flattened_centroids = mesh->get_attribute("face_centroid");
    MatrixFr centroids(num_faces, 3);
    std::copy(flattened_centroids.data(),
            flattened_centroids.data() + num_faces * 3,
            centroids.data());

    VectorF flattened_vertices = mesh->get_vertices();
    MatrixFr vertices(num_vertices, dim);
    std::copy( flattened_vertices.data(),
            flattened_vertices.data() + num_vertices * dim,
            vertices.data());

    VectorI flattened_faces = mesh->get_faces();
    MatrixIr faces(num_faces, 3);
    std::copy( flattened_faces.data(),
            flattened_faces.data() + num_faces* 3,
            faces.data());

    AABBTree tree(vertices, faces);
    VectorF distances;
    VectorI face_indices;
    MatrixFr closest_points;
    tree.look_up_with_closest_points(
            centroids, distances, face_indices, closest_points);

    ASSERT_EQ(num_faces, distances.size());
    ASSERT_EQ(num_faces, face_indices.size());
    ASSERT_EQ(num_faces, closest_points.rows());

    for (size_t i=0; i<num_faces; i++) {
        ASSERT_EQ(i, face_indices[i]);
        ASSERT_FLOAT_EQ(0.0, (centroids.row(i) - closest_points.row(i)).norm());
    }
}
