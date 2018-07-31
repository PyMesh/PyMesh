/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM
#include <string>
#include <memory>
#include <Geogram/GeogramBase.h>
#include <Geogram/GeogramMeshIO.h>
#include <Geogram/GeogramMeshUtils.h>

#include <TestBase.h>

class GeogramMeshIOTest : public TestBase {
    protected:
        virtual void SetUp() {
            TestBase::SetUp();
            GeogramBase geo;
        }

        GeoMeshPtr write_and_load(const std::string& filename, GeoMeshPtr mesh) {
            GeogramMeshIO::save_mesh(filename, mesh);
            auto r = GeogramMeshIO::load_mesh(filename);
            remove(filename);
            return r;
        }

        void remove(const std::string& filename) {
            std::string full_name = filename;
            int r = std::remove(full_name.c_str());
            ASSERT_EQ(0, r);
        }
};

TEST_F(GeogramMeshIOTest, cube) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto geo_mesh = GeogramMeshUtils::mesh_to_geomesh(mesh);
    auto geo_mesh_2 = write_and_load("cube.geogram", geo_mesh);
    MeshPtr mesh2 = GeogramMeshUtils::geomesh_to_mesh(geo_mesh_2);

    ASSERT_EQ(mesh->get_dim(), mesh2->get_dim());
    ASSERT_EQ(mesh->get_num_vertices(), mesh2->get_num_vertices());
    ASSERT_EQ(mesh->get_num_faces(), mesh2->get_num_faces());

    ASSERT_MATRIX_EQ(mesh->get_vertices(), mesh2->get_vertices());
    ASSERT_MATRIX_EQ(mesh->get_faces(), mesh2->get_faces());
}

TEST_F(GeogramMeshIOTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    auto geo_mesh = GeogramMeshUtils::mesh_to_geomesh(mesh);
    auto geo_mesh_2 = write_and_load("tet.geogram", geo_mesh);
    MeshPtr mesh2 = GeogramMeshUtils::geomesh_to_mesh(geo_mesh_2);

    ASSERT_EQ(mesh->get_dim(), mesh2->get_dim());
    ASSERT_EQ(mesh->get_num_vertices(), mesh2->get_num_vertices());
    ASSERT_EQ(mesh->get_num_faces(), mesh2->get_num_faces());

    ASSERT_MATRIX_EQ(mesh->get_vertices(), mesh2->get_vertices());
    ASSERT_MATRIX_EQ(mesh->get_faces(), mesh2->get_faces());
}

/**
 * Geogram (v1.6.3) crashes when loading zero vertices.
 */
TEST_F(GeogramMeshIOTest, empty) {
    MeshPtr mesh = load_mesh("empty.msh");
    auto geo_mesh = GeogramMeshUtils::mesh_to_geomesh(mesh);
    auto geo_mesh_2 = write_and_load("empty.geogram", geo_mesh);
    MeshPtr mesh2 = GeogramMeshUtils::geomesh_to_mesh(geo_mesh_2);

    ASSERT_EQ(mesh->get_dim(), mesh2->get_dim());
    ASSERT_EQ(0, mesh2->get_num_vertices());
    ASSERT_EQ(0, mesh2->get_num_faces());
}

#endif
