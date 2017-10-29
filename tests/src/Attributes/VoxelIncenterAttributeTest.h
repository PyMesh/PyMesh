/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class VoxelIncenterAttributeTest : public TestBase {
};

TEST_F(VoxelIncenterAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("voxel_incenter");
    ASSERT_TRUE(mesh->has_attribute("voxel_incenter"));
    VectorF incenters = mesh->get_attribute("voxel_incenter");
    ASSERT_EQ(3, incenters.size());

    mesh->add_attribute("face_normal");
    VectorF face_normals = mesh->get_attribute("face_normal");
    ASSERT_EQ(12, face_normals.size());
    Vector3F n0 = face_normals.segment<3>(0);
    Vector3F n1 = face_normals.segment<3>(1*3);
    Vector3F n2 = face_normals.segment<3>(2*3);
    Vector3F n3 = face_normals.segment<3>(3*3);

    const auto& faces = mesh->get_faces();

    Float dist_0 = (incenters - mesh->get_vertex(faces[0  ])).dot(n0);
    Float dist_1 = (incenters - mesh->get_vertex(faces[1*3])).dot(n1);
    Float dist_2 = (incenters - mesh->get_vertex(faces[2*3])).dot(n2);
    Float dist_3 = (incenters - mesh->get_vertex(faces[3*3])).dot(n3);

    ASSERT_FLOAT_EQ(dist_0, dist_1);
    ASSERT_FLOAT_EQ(dist_1, dist_2);
    ASSERT_FLOAT_EQ(dist_2, dist_3);
    ASSERT_FLOAT_EQ(dist_3, dist_0);
}

TEST_F(VoxelIncenterAttributeTest, irregular_tet) {
    MatrixFr raw_vertices(4, 3);
    raw_vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.5, 1.0, 0.0,
                0.5, 0.0, -1.0;
    MatrixIr raw_faces(0, 3);
    MatrixIr raw_voxels(1, 4);
    raw_voxels << 0,1,2,3;
    MeshPtr mesh = load_data(raw_vertices, raw_faces, raw_voxels);

    mesh->add_attribute("voxel_incenter");
    ASSERT_TRUE(mesh->has_attribute("voxel_incenter"));
    VectorF incenters = mesh->get_attribute("voxel_incenter");
    ASSERT_EQ(3, incenters.size());

    mesh->add_attribute("face_normal");
    VectorF face_normals = mesh->get_attribute("face_normal");
    ASSERT_EQ(12, face_normals.size());
    Vector3F n0 = face_normals.segment<3>(0);
    Vector3F n1 = face_normals.segment<3>(1*3);
    Vector3F n2 = face_normals.segment<3>(2*3);
    Vector3F n3 = face_normals.segment<3>(3*3);

    const auto& faces = mesh->get_faces();

    Float dist_0 = (incenters - mesh->get_vertex(faces[0  ])).dot(n0);
    Float dist_1 = (incenters - mesh->get_vertex(faces[1*3])).dot(n1);
    Float dist_2 = (incenters - mesh->get_vertex(faces[2*3])).dot(n2);
    Float dist_3 = (incenters - mesh->get_vertex(faces[3*3])).dot(n3);

    ASSERT_FLOAT_EQ(dist_0, dist_1);
    ASSERT_FLOAT_EQ(dist_1, dist_2);
    ASSERT_FLOAT_EQ(dist_2, dist_3);
    ASSERT_FLOAT_EQ(dist_3, dist_0);
}
