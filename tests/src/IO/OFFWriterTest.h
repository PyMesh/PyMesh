/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <memory>

#include <IO/MeshWriter.h>
#include "WriterTest.h"

class OFFWriterTest : public WriterTest {
    protected:
        MeshPtr write_and_load(const std::string& filename, MeshPtr mesh) {
            write_tmp_mesh(filename, mesh);
            MeshPtr r = load_tmp_mesh(filename);
            remove(filename);
            return r;
        }

        MeshPtr write_and_load_raw(const std::string& filename, MeshPtr mesh) {
            write_tmp_mesh_raw(
                    filename,
                    mesh->get_vertices(),
                    mesh->get_faces(),
                    mesh->get_voxels(),
                    mesh->get_dim(),
                    mesh->get_vertex_per_face(),
                    mesh->get_vertex_per_voxel());
            return load_tmp_mesh(filename);
        }
};

TEST_F(OFFWriterTest, EmptyMesh) {
    MeshPtr m1 = load_mesh("empty.off");
    MeshPtr m2 = write_and_load("empty.off", m1);

    assert_eq_vertices(m1, m2);
    assert_eq_faces(m1, m2);
}

TEST_F(OFFWriterTest, Cube) {
    MeshPtr m1 = load_mesh("cube.obj");
    MeshPtr m2 = write_and_load("cube.off", m1);

    assert_eq_vertices(m1, m2);
    assert_eq_faces(m1, m2);
}

TEST_F(OFFWriterTest, 2D) {
    MeshPtr m1 = load_mesh("square_2D.obj");
    MeshPtr m2 = write_and_load("square_2D.off", m1);

    assert_eq_vertices(m1, m2);
    assert_eq_faces(m1, m2);
}

TEST_F(OFFWriterTest, quad) {
    MeshPtr m1 = load_mesh("quad.obj");
    MeshPtr m2 = write_and_load("quad.off", m1);

    assert_eq_vertices(m1, m2);
    assert_eq_faces(m1, m2);
}
