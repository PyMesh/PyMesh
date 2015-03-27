/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <set>

#include <Core/Exception.h>

#include <MeshUtils/Subdivision.h>

#include <TestBase.h>

class SubdivisionTest : public TestBase {
    protected:
        typedef Subdivision::Ptr SubDivPtr;

        MeshPtr load_mesh(const std::string& filename) {
            MeshPtr r = TestBase::load_mesh(filename);
            r->add_attribute("face_area");
            return r;
        }

        virtual SubDivPtr create_subdivision() {
            throw NotImplementedError("This test method needs to be implemented in subclass");
        }

        MatrixFr extract_vertices(MeshPtr mesh) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();
            VectorF flattened_vertices = mesh->get_vertices();
            Eigen::Map<MatrixFr> vertices(flattened_vertices.data(), num_vertices, dim);
            return vertices;
        }

        MatrixIr extract_faces(MeshPtr mesh) {
            const size_t num_faces = mesh->get_num_faces();
            const size_t num_vertex_per_face = mesh->get_vertex_per_face();
            VectorI flattened_faces = mesh->get_faces();
            Eigen::Map<MatrixIr> faces(flattened_faces.data(), num_faces, num_vertex_per_face);
            return faces;
        }

        void assert_equal_centroid(const MatrixFr& pts1, const MatrixFr& pts2) {
            const size_t dim = pts1.cols();
            ASSERT_EQ(dim, pts2.cols());

            VectorF centroid1 = VectorF::Zero(dim);
            VectorF centroid2 = VectorF::Zero(dim);

            for (size_t i=0; i<pts1.rows(); i++) {
                VectorF p = pts1.row(i);
                centroid1 = centroid1 + p;
            }
            centroid1 = centroid1 / pts1.rows();

            for (size_t i=0; i<pts1.rows(); i++) {
                VectorF p = pts2.row(i);
                centroid2 = centroid2 + p;
            }
            centroid2 = centroid2 / pts2.rows();

            Float dist = (centroid1 - centroid2).norm();
            ASSERT_NEAR(0.0, dist, 1e-6);
        }
};

