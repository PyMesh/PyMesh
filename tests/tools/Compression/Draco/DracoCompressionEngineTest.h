/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Compression/CompressionEngine.h>

#include <TestBase.h>

using namespace PyMesh;

class DracoCompressionEngineTest : public TestBase {
};

TEST_F(DracoCompressionEngineTest, Simple) {
    auto mesh = load_mesh("ball.msh");
    if (CompressionEngine::supports("draco")) {
        auto engine = CompressionEngine::create("draco");
        auto data = engine->compress(mesh);
        auto mesh2 = engine->decompress(data);

        ASSERT_EQ(mesh->get_num_vertices(), mesh2->get_num_vertices());
        ASSERT_EQ(mesh->get_num_faces(), mesh2->get_num_faces());
        ASSERT_EQ(mesh->get_num_voxels(), mesh2->get_num_voxels());

        // Both vertices and faces are reordered.
        //const auto& faces = mesh->get_faces();
        //const auto& faces2 = mesh2->get_faces();
        //ASSERT_MATRIX_EQ(faces, faces2); 
    }
}
