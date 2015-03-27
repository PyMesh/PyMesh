/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <TestBase.h>

class OFFParserTest : public TestBase {
    protected:
        void parse(const std::string& mesh_file) {
            m_parser = std::shared_ptr<MeshParser>(MeshParser::create_parser(mesh_file));
            bool result = m_parser->parse(mesh_file);
            ASSERT_TRUE(result);
        }

    protected:
        std::shared_ptr<MeshParser> m_parser;
};

TEST_F(OFFParserTest, ParseCube) {
    std::string mesh_file = m_data_dir + "cube.off";
    parse(mesh_file);
    ASSERT_EQ(8,  m_parser->num_vertices());
    ASSERT_EQ(12, m_parser->num_faces());
    ASSERT_EQ(0,  m_parser->num_voxels());
    ASSERT_EQ(0,  m_parser->vertex_per_voxel());
    ASSERT_EQ(3,  m_parser->vertex_per_face());
    ASSERT_EQ(3,  m_parser->dim());
}
