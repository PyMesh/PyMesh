/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <algorithm>
#include <string>

#include <Core/EigenTypedef.h>
#include <Boolean/BooleanEngine.h>
#include <IO/MeshWriter.h>

#include <TestBase.h>

class BooleanEngineTest : public TestBase {
    protected:
        typedef BooleanEngine::Ptr BooleanPtr;

        void save_mesh(const std::string& filename,
                const MatrixFr& vertices, const MatrixIr& faces) {
            write_mesh_raw(filename, vertices, faces);
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
};
