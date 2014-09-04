#pragma once

#include <algorithm>
#include <string>

#include <Core/EigenTypedef.h>
#include <CSG/CSGEngine.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <IO/MeshWriter.h>
#include <Misc/Environment.h>

class CSGEngineTest : public ::testing::Test {
    protected:
        typedef Mesh::Ptr MeshPtr;
        typedef CSGEngine::Ptr CSGPtr;

        virtual void SetUp() {
            std::string proj_root =
                Environment::get_required("PYMESH_PATH");
            m_data_dir = proj_root + "/tests/data/";
        }

        MeshPtr load_mesh(const std::string& filename) {
            std::string mesh_file = m_data_dir + filename;
            return MeshPtr(
                    MeshFactory()
                    .load_file(mesh_file)
                    .create());
        }

        void save_mesh(const std::string& filename,
                const MatrixFr& vertices, const MatrixIr& faces) {
            MeshWriter* writer = MeshWriter::create_writer(filename);

            const size_t num_v_entries = vertices.rows() * vertices.cols();
            VectorF linearized_vertices(num_v_entries);
            std::copy(vertices.data(), vertices.data() + num_v_entries,
                    linearized_vertices.data());

            const size_t num_f_entries = faces.rows() * faces.cols();
            VectorI linearized_faces(num_f_entries);
            std::copy(faces.data(), faces.data() + num_f_entries,
                    linearized_faces.data());

            VectorI voxels = VectorI::Zero(0);
            writer->write(linearized_vertices, linearized_faces, voxels,
                    vertices.cols(), faces.cols(), 0);
            delete writer;
        }

        MatrixFr extract_vertices(MeshPtr mesh) {
            MatrixFr out_vertices(mesh->get_num_vertices(), mesh->get_dim());
            const VectorF& vertices = mesh->get_vertices();
            std::copy(vertices.data(), vertices.data() + vertices.size(),
                    out_vertices.data());
            return out_vertices;
        }

        MatrixIr extract_faces(MeshPtr mesh) {
            MatrixIr out_faces(mesh->get_num_faces(), mesh->get_vertex_per_face());
            const VectorI& faces = mesh->get_faces();
            std::copy(faces.data(), faces.data() + faces.size(),
                    out_faces.data());
            return out_faces;
        }

        void translate(MatrixFr& pts, const VectorF& offset) {
            const size_t num_pts = pts.rows();
            for (size_t i=0; i<num_pts; i++) {
                pts.row(i) += offset;
            }
        }

    protected:
        std::string m_data_dir;
};
