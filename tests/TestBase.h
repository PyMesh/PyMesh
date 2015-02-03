#pragma once

#include <string>

#include <Mesh.h>
#include <MeshFactory.h>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <IO/MeshWriter.h>
#include <Misc/Environment.h>

/**
 * Base test class.  This class provides some common method often needed during
 * unit testing.
 */
class TestBase : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;

        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/data/";
        }

        virtual MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshFactory().load_file(mesh_file).create_shared();
        }

        virtual MeshPtr load_data(
                VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
            return MeshFactory().load_data(vertices, faces, voxels,
                        dim, vertex_per_face, vertex_per_voxel).create_shared();
        }

        virtual void write_mesh(const std::string& mesh_file, MeshPtr mesh) {
            MeshWriter::Ptr writer = MeshWriter::create(mesh_file);
            writer->write_mesh(*mesh);
        }

        virtual void write_mesh_raw(const std::string& mesh_file,
                VectorF& vertices, VectorI& faces, VectorI& voxels,
                size_t dim, size_t vertex_per_face, size_t vertex_per_voxel) {
            MeshWriter::Ptr writer = MeshWriter::create(mesh_file);
            writer->write(vertices, faces, voxels,
                    dim, vertex_per_face, vertex_per_voxel);
        }

    protected:
        /**
         * Compare two matrices entry by entry.
         */
        template<typename Derived1, typename Derived2>
        void ASSERT_MATRIX_EQ(
                const Eigen::MatrixBase<Derived1>& M1,
                const Eigen::MatrixBase<Derived2>& M2) {
            ASSERT_FLOAT_EQ(0.0, (M1 - M2).squaredNorm());
        }

    protected:
        std::string m_data_dir;
};
