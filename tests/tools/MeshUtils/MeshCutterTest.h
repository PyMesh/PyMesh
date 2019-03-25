/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <MeshUtils/MeshCutter.h>

class MeshCutterTest : public TestBase {

};

TEST_F(MeshCutterTest, empty) {
    MeshPtr mesh = load_mesh("empty.ply");

    std::vector<size_t> ids;
    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_with_face_labels(ids);

    ASSERT_EQ(0, cutted_mesh->get_num_vertices());
    ASSERT_EQ(0, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, SingleTriangle) {
    VectorF vertices(9);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    VectorI faces(3);
    faces << 0, 1, 2;
    VectorI voxels(0);
    MeshPtr mesh = MeshFactory().load_data(vertices, faces, voxels, 3, 3, 4).create();

    std::vector<size_t> ids(1, 0);

    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_with_face_labels(ids);

    ASSERT_EQ(3, cutted_mesh->get_num_vertices());
    ASSERT_EQ(1, cutted_mesh->get_num_faces());
    ASSERT_MATRIX_EQ(vertices, cutted_mesh->get_vertices());
    ASSERT_MATRIX_EQ(faces, cutted_mesh->get_faces());
}

TEST_F(MeshCutterTest, TwoTriangles) {
    VectorF vertices(12);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;
    VectorI faces(6);
    faces << 0, 1, 2,
             1, 0, 3;
    VectorI voxels(0);
    MeshPtr mesh = MeshFactory().load_data(vertices, faces, voxels, 3, 3, 4).create();

    std::vector<size_t> ids(2, 0);
    ids[1] = 1;

    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_with_face_labels(ids);

    ASSERT_EQ(6, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, square) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_with_face_labels({0, 1});

    ASSERT_EQ(6, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());

    cutted_mesh = cutter.cut_with_face_labels({1, 1});
    ASSERT_EQ(4, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, quad_s1) {
    MeshPtr mesh = load_mesh("quad_s1.obj");
    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_with_face_labels({0, 0, 1, 1});

    ASSERT_EQ(12, cutted_mesh->get_num_vertices());
    ASSERT_EQ(4, cutted_mesh->get_num_faces());

    cutted_mesh = cutter.cut_with_face_labels({0, 2, 3, 4});
    ASSERT_EQ(16, cutted_mesh->get_num_vertices());
    ASSERT_EQ(4, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, uv) {
    VectorF vertices(12);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                1.0, 1.0, 1.0;
    VectorI faces(6);
    faces << 0, 1, 2,
             2, 1, 3;
    VectorI voxels(0);
    MeshPtr mesh = MeshFactory().load_data(vertices, faces, voxels, 3, 3, 4).create();

    VectorF uvs(12);
    uvs << 0.0, 0.0,
           1.0, 0.0,
           0.0, 1.0,
           0.0, 1.0,
           1.0, 0.0,
           1.0, 1.0;
    mesh->add_attribute("corner_texture");
    mesh->set_attribute("corner_texture", uvs);

    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_at_uv_discontinuity();
    ASSERT_EQ(4, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());

    // Split uv at vertex 1.
    uvs.segment<2>(2) << 0.9, 0.0;
    mesh->set_attribute("corner_texture", uvs);
    cutted_mesh = cutter.cut_at_uv_discontinuity();
    ASSERT_EQ(5, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());

    // Split uv at vertex 2 as well.
    uvs.segment<2>(4) << -0.1, 0.9;
    mesh->set_attribute("corner_texture", uvs);
    cutted_mesh = cutter.cut_at_uv_discontinuity();
    ASSERT_EQ(6, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, hex_s1) {
    constexpr Float eps = 1e-3;

    MeshPtr mesh = load_mesh("hex_s1.msh");
    mesh->add_attribute("face_normal");
    const auto& normals = mesh->get_attribute("face_normal");

    const size_t num_faces = mesh->get_num_faces();
    VectorF uv(num_faces * 8);

    const auto& vertices = mesh->get_vertices();
    for (size_t i=0; i<num_faces; i++) {
        const auto f = mesh->get_face(i);
        const Vector3F n = normals.segment<3>(i*3);
        if (n[0] <= -1+eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+2), vertices(f[0]*3+1),
                vertices(f[1]*3+2), vertices(f[1]*3+1),
                vertices(f[2]*3+2), vertices(f[2]*3+1),
                vertices(f[3]*3+2), vertices(f[3]*3+1);
        } else if (n[0] >= 1-eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+1), vertices(f[0]*3+2) + 10,
                vertices(f[1]*3+1), vertices(f[1]*3+2) + 10,
                vertices(f[2]*3+1), vertices(f[2]*3+2) + 10,
                vertices(f[3]*3+1), vertices(f[3]*3+2) + 10;
        } else if (n[1] <= -1+eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+0), vertices(f[0]*3+2) + 20,
                vertices(f[1]*3+0), vertices(f[1]*3+2) + 20,
                vertices(f[2]*3+0), vertices(f[2]*3+2) + 20,
                vertices(f[3]*3+0), vertices(f[3]*3+2) + 20;
        } else if (n[1] >= 1-eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+2), vertices(f[0]*3+0) + 30,
                vertices(f[1]*3+2), vertices(f[1]*3+0) + 30,
                vertices(f[2]*3+2), vertices(f[2]*3+0) + 30,
                vertices(f[3]*3+2), vertices(f[3]*3+0) + 30;
        } else if (n[2] <= -1+eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+1), vertices(f[0]*3+0) + 40,
                vertices(f[1]*3+1), vertices(f[1]*3+0) + 40,
                vertices(f[2]*3+1), vertices(f[2]*3+0) + 40,
                vertices(f[3]*3+1), vertices(f[3]*3+0) + 40;
        } else if (n[2] >= 1-eps) {
            uv.segment<8>(i*4*2) <<
                vertices(f[0]*3+0), vertices(f[0]*3+1) + 50,
                vertices(f[1]*3+0), vertices(f[1]*3+1) + 50,
                vertices(f[2]*3+0), vertices(f[2]*3+1) + 50,
                vertices(f[3]*3+0), vertices(f[3]*3+1) + 50;
        } else {
            FAIL() << "Unexpected face normal in a cube: "
                << n[0] << ", " << n[1] << ", " << n[2];
        }
    }

    mesh->add_attribute("corner_texture");
    mesh->set_attribute("corner_texture", uv);

    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_at_uv_discontinuity();

    // 9 vertex per side, 6 sides in total, plus one isolated vertex in the
    // center of the cube.  Thus, 55 vertices in total.
    ASSERT_EQ(55, cutted_mesh->get_num_vertices());
    ASSERT_EQ(num_faces, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, quad) {
    VectorF vertices(12);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;
    VectorI faces(6);
    faces << 0, 1, 2,
             1, 0, 3;
    VectorI voxels;
    MeshPtr mesh = MeshFactory().load_data(vertices, faces, voxels, 3, 3, 4).create();

    std::vector<std::vector<int>> cut{{0, 1}};
    MeshCutter cutter(mesh);
    MeshPtr cutted_mesh = cutter.cut_along_edges(cut);

    ASSERT_EQ(6, cutted_mesh->get_num_vertices());
    ASSERT_EQ(2, cutted_mesh->get_num_faces());
}

TEST_F(MeshCutterTest, 3x3_quad) {
    VectorF vertices(32);
    vertices << 0.0, 0.0,
                1.0, 0.0,
                2.0, 0.0,
                3.0, 0.0,
                0.0, 1.0,
                1.0, 1.0,
                2.0, 1.0,
                3.0, 1.0,
                0.0, 2.0,
                1.0, 2.0,
                2.0, 2.0,
                3.0, 2.0,
                0.0, 3.0,
                1.0, 3.0,
                2.0, 3.0,
                3.0, 3.0;
    VectorI faces(18*3);
    faces <<
        0, 1, 5,
        0, 5, 4,
        1, 2, 6,
        1, 6, 5,
        2, 3, 7,
        2, 7, 6,
        4, 5, 9,
        4, 9, 8,
        5, 6, 10,
        5, 10, 9,
        6, 7, 11,
        6, 11, 10,
        8, 9, 13,
        8, 13, 12,
        9, 10, 14,
        9, 14, 13,
        10, 11, 15,
        10, 15, 14;

    //  12    13    14    15
    //   +-----+-----+-----+
    //   |   / |   / |   / |
    //   | /  9| / 10| / 11|
    // 8 +-----+-----+-----+
    //   |   / |   / |   / |
    //   | /  5| /  6| /  7|
    // 4 +-----+-----+-----+
    //   |   / |   / |   / |
    //   | /  1| /  2| /  3|
    // 0 +-----+-----+-----+

    VectorI voxels;
    MeshPtr mesh = MeshFactory().load_data(vertices, faces, voxels, 2, 3, 4).create();
    MeshCutter cutter(mesh);

    {
        std::vector<std::vector<int>> cut{{1, 5, 9, 13}};
        MeshPtr cutted_mesh = cutter.cut_along_edges(cut);
        ASSERT_EQ(16+4, cutted_mesh->get_num_vertices());
    }

    {
        std::vector<std::vector<int>> cut{{0, 1, 2, 3}};
        MeshPtr cutted_mesh = cutter.cut_along_edges(cut);
        ASSERT_EQ(16, cutted_mesh->get_num_vertices());
    }

    {
        std::vector<std::vector<int>> cut{{0, 5, 10, 15}};
        MeshPtr cutted_mesh = cutter.cut_along_edges(cut);
        ASSERT_EQ(16+4, cutted_mesh->get_num_vertices());
    }

    {
        std::vector<std::vector<int>> cut{{4, 5, 6, 7}, {0, 5, 10, 15}};
        MeshPtr cutted_mesh = cutter.cut_along_edges(cut);
        ASSERT_EQ(16+9, cutted_mesh->get_num_vertices());
    }
}

