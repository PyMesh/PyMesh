/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>
#include <IO/MeshParser.h>
#include <TestBase.h>

class OBJParserTest : public TestBase {
    protected:
        void parse(const std::string& mesh_file) {
            m_parser = std::shared_ptr<MeshParser>(MeshParser::create_parser(mesh_file));
            bool result = m_parser->parse(mesh_file);
            ASSERT_TRUE(result);
        }

    protected:
        std::shared_ptr<MeshParser> m_parser;
};

TEST_F(OBJParserTest, ParseCube) {
    std::string mesh_file = m_data_dir + "cube.obj";
    parse(mesh_file);
    ASSERT_EQ(8,  m_parser->num_vertices());
    ASSERT_EQ(12, m_parser->num_faces());
    ASSERT_EQ(0,  m_parser->num_voxels());
    ASSERT_EQ(0,  m_parser->vertex_per_voxel());
    ASSERT_EQ(3,  m_parser->vertex_per_face());
    ASSERT_EQ(3,  m_parser->dim());
}

TEST_F(OBJParserTest, ParseTet) {
    std::string mesh_file = m_data_dir + "tet.obj";
    parse(mesh_file);

    ASSERT_EQ(4, m_parser->num_vertices());
    ASSERT_EQ(4, m_parser->num_faces());
    ASSERT_EQ(0, m_parser->num_voxels());
    ASSERT_EQ(0, m_parser->vertex_per_voxel());
    ASSERT_EQ(3, m_parser->vertex_per_face());
    ASSERT_EQ(3, m_parser->dim());
}

TEST_F(OBJParserTest, 2DSquare) {
    std::string mesh_file = m_data_dir + "square_2D.obj";
    parse(mesh_file);
    ASSERT_EQ(4, m_parser->num_vertices());
    ASSERT_EQ(2, m_parser->num_faces());
    ASSERT_EQ(0, m_parser->num_voxels());
    ASSERT_EQ(0, m_parser->vertex_per_voxel());
    ASSERT_EQ(3, m_parser->vertex_per_face());
    ASSERT_EQ(2, m_parser->dim());
}

TEST_F(OBJParserTest, Quad) {
    std::string mesh_file = m_data_dir + "quad.obj";
    parse(mesh_file);
    ASSERT_EQ(4, m_parser->num_vertices());
    ASSERT_EQ(1, m_parser->num_faces());
    ASSERT_EQ(0, m_parser->num_voxels());
    ASSERT_EQ(0, m_parser->vertex_per_voxel());
    ASSERT_EQ(4, m_parser->vertex_per_face());
    ASSERT_EQ(2, m_parser->dim());
}

TEST_F(OBJParserTest, TetCentroid) {
    std::string mesh_file = m_data_dir + "tet.obj";
    parse(mesh_file);

    const size_t num_vertices = m_parser->num_vertices();
    const size_t dim = m_parser->dim();

    VectorF vertices(num_vertices * dim);
    m_parser->export_vertices(vertices.data());
    Float coord_sum = vertices.sum();
    ASSERT_NEAR(0.0, coord_sum, 1e-6);
}

TEST_F(OBJParserTest, TetValance) {
    std::string mesh_file = m_data_dir + "tet.obj";
    parse(mesh_file);

    const size_t num_vertices = m_parser->num_vertices();
    const size_t num_faces = m_parser->num_faces();
    const size_t vertex_per_face = m_parser->vertex_per_face();

    VectorI faces(num_faces * vertex_per_face);
    m_parser->export_faces(faces.data());
    std::vector<int> valance(num_vertices, 0);
    for (size_t i=0; i<faces.size(); i++) {
        ASSERT_LT(faces[i], num_faces);
        valance[faces[i]] += 1;
    }
    for (size_t i=0; i<valance.size(); i++) {
        ASSERT_EQ(3, valance[i]);
    }
}

TEST_F(OBJParserTest, 2DSquareCentroid) {
    std::string mesh_file = m_data_dir + "square_2D.obj";
    parse(mesh_file);

    const size_t num_vertices = m_parser->num_vertices();
    const size_t dim = m_parser->dim();

    VectorF vertices(num_vertices * dim);
    m_parser->export_vertices(vertices.data());
    Float coord_sum = vertices.sum();
    ASSERT_NEAR(0.0, coord_sum, 1e-6);
}

TEST_F(OBJParserTest, Empty) {
    std::string mesh_file = m_data_dir + "empty.obj";
    parse(mesh_file);
    const size_t num_vertices = m_parser->num_vertices();

    ASSERT_EQ(0, m_parser->num_vertices());
    ASSERT_EQ(0, m_parser->num_faces());
}

