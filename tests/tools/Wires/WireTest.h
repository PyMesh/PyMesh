/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>

#include <IO/MeshWriter.h>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Math/MatrixUtils.h>

#include <Wires/WireNetwork/WireNetwork.h>

#include <TestBase.h>

class WireTest : public TestBase {
    protected:
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/tools/Wires/data/";
        }

        WireNetwork load_wire(const std::string& filename) {
            return WireNetwork(m_data_dir + filename);
        }

        WireNetwork::Ptr load_wire_shared(const std::string& filename) {
            return WireNetwork::create(m_data_dir + filename);
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_MATRIX_EQ(v1, v2);
        }

        template<typename T>
        Eigen::Matrix<T, Eigen::Dynamic, 1> flatten(
                const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& mat) {
            return MatrixUtils::flatten<Eigen::Matrix<T, Eigen::Dynamic, 1> >(mat);
        }

        Mesh::Ptr form_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
            const size_t num_vertices = vertices.rows();
            const size_t dim = vertices.cols();
            const size_t num_faces = faces.rows();
            const size_t vertex_per_face = faces.cols();

            VectorF flattened_vertices = Eigen::Map<const VectorF>(
                    vertices.data(), num_vertices * dim);
            VectorI flattened_faces = Eigen::Map<const VectorI>(
                    faces.data(), vertex_per_face * num_faces);
            VectorI voxels = VectorI::Zero(0);

            MeshFactory factory;
            factory.load_data(flattened_vertices, flattened_faces, voxels,
                    dim, vertex_per_face, 0);
            return factory.create_shared();
        }

        void save_mesh(const std::string& file_name,
                MatrixFr& vertices, MatrixIr& faces,
                VectorF debug=VectorF::Zero(0)) {
            VectorF flat_vertices = flatten(vertices);
            VectorI flat_faces = flatten(faces);
            VectorI flat_voxels = VectorI::Zero(0);

            MeshFactory factory;
            Mesh::Ptr mesh = factory.load_data(flat_vertices, flat_faces, flat_voxels,
                    vertices.cols(), faces.cols(), 4).create_shared();

            MeshWriter* writer = MeshWriter::create_writer(file_name);

            if (debug.size() > 0) {
                mesh->add_attribute("debug");
                mesh->set_attribute("debug", debug);
                writer->with_attribute("debug");
            }

            writer->write_mesh(*mesh);
            delete writer;
        }

        void save_mesh(const std::string& filename,
                Mesh::Ptr mesh,
                const std::string& attr_name) {
            std::vector<std::string> attr_names;
            attr_names.push_back(attr_name);
            save_mesh(filename, mesh, attr_names);
        }

        void save_mesh(const std::string& file_name,
                Mesh::Ptr mesh,
                const std::vector<std::string>& attr_names) {
            MeshFactory factory;
            MeshWriter* writer = MeshWriter::create_writer(file_name);

            for (const auto& attr_name : attr_names) {
                writer->with_attribute(attr_name);
            }
            writer->write_mesh(*mesh);
            delete writer;
        }
};
