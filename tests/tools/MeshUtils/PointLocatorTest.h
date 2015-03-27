/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/Exception.h>

#include <MeshUtils/PointLocator.h>

#include <TestBase.h>

class PointLocatorTest : public TestBase {
    protected:
        MatrixF uniform_samples(size_t num_samples, VectorF bbox_min, VectorF bbox_max) {
            const size_t dim = bbox_min.size();
            MatrixF samples(int(pow(num_samples, dim)), dim);
            VectorF step = (bbox_max-bbox_min) / (num_samples-1);

            if (dim == 3) {
                for (size_t i=0; i<num_samples; i++) {
                    for (size_t j=0; j<num_samples; j++) {
                        for (size_t k=0; k<num_samples; k++) {
                            size_t index = (i*num_samples+j)*num_samples+k;
                            VectorF multiple(3);
                            multiple[0] = i;
                            multiple[1] = j;
                            multiple[2] = k;
                            samples.row(index) = bbox_min + multiple.cwiseProduct(step);
                        }
                    }
                }
            } else if (dim == 2) {
                for (size_t i=0; i<num_samples; i++) {
                    for (size_t j=0; j<num_samples; j++) {
                        size_t index = i*num_samples+j;
                        VectorF multiple(2);
                        multiple[0] = i;
                        multiple[1] = j;
                        samples.row(index) = bbox_min + multiple.cwiseProduct(step);
                    }
                }
            }
            return samples;
        }

        void check_barycentric_coord(MeshPtr mesh, VectorF target,
                VectorI elem, VectorF barycentric_coord) {
            ASSERT_EQ(elem.size(), barycentric_coord.size());
            size_t vertex_per_elem = elem.size();

            VectorF p = VectorF::Zero(mesh->get_dim());
            for (size_t i=0; i<vertex_per_elem; i++) {
                p = p + mesh->get_vertex(elem[i]) * barycentric_coord[i];
            }

            Float dist = (p - target).norm();
            ASSERT_NEAR(0.0, dist, 1e-6);
        }
};

TEST_F(PointLocatorTest, 2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    PointLocator locator(mesh);
    MatrixF pts = uniform_samples(5, -1*VectorF::Ones(2), VectorF::Ones(2));
    size_t num_pts = pts.rows();

    locator.locate(pts);
    VectorI elem_indices = locator.get_enclosing_voxels();
    MatrixF barycentric_coords = locator.get_barycentric_coords();

    ASSERT_EQ(num_pts, elem_indices.size());
    ASSERT_EQ(num_pts, barycentric_coords.rows());

    for (size_t i=0; i<num_pts; i++) {
        VectorI face = mesh->get_face(elem_indices[i]);
        check_barycentric_coord(mesh, pts.row(i), face,
                barycentric_coords.row(i));
    }
}

TEST_F(PointLocatorTest, 3D) {
    MeshPtr mesh = load_mesh("cube.msh");
    PointLocator locator(mesh);
    MatrixF pts = uniform_samples(5, -1*VectorF::Ones(3), VectorF::Ones(3));
    size_t num_pts = pts.rows();

    locator.locate(pts);
    VectorI elem_indices = locator.get_enclosing_voxels();
    MatrixF barycentric_coords = locator.get_barycentric_coords();

    ASSERT_EQ(num_pts, elem_indices.size());
    ASSERT_EQ(num_pts, barycentric_coords.rows());

    for (size_t i=0; i<num_pts; i++) {
        VectorI voxel = mesh->get_voxel(elem_indices[i]);
        check_barycentric_coord(mesh, pts.row(i), voxel,
                barycentric_coords.row(i));
    }
}

TEST_F(PointLocatorTest, Outside) {
    MeshPtr mesh = load_mesh("tet.msh");
    PointLocator locator(mesh);
    MatrixF pts = uniform_samples(5, -1*VectorF::Ones(3), VectorF::Ones(3));
    size_t num_pts = pts.rows();

    ASSERT_THROW(locator.locate(pts), RuntimeError);
}

TEST_F(PointLocatorTest, SlightlyOutside) {
    MeshPtr mesh = load_mesh("cube.msh");
    PointLocator locator(mesh);
    MatrixF pts = MatrixF::Ones(1, 3) * 1.01;
    size_t num_pts = pts.rows();

    locator.locate(pts);
    VectorI elem_indices = locator.get_enclosing_voxels();
    MatrixF barycentric_coords = locator.get_barycentric_coords();

    ASSERT_EQ(num_pts, elem_indices.size());
    ASSERT_EQ(num_pts, barycentric_coords.rows());

    for (size_t i=0; i<num_pts; i++) {
        VectorI voxel = mesh->get_voxel(elem_indices[i]);
        check_barycentric_coord(mesh, pts.row(i), voxel,
                barycentric_coords.row(i));
        ASSERT_GE(0.0, barycentric_coords.row(i).minCoeff());
    }
}

TEST_F(PointLocatorTest, MidFacePts) {
    MeshPtr mesh = load_mesh("cube.msh");
    mesh->add_attribute("face_centroid");
    PointLocator locator(mesh);
    VectorF centroids = mesh->get_attribute("face_centroid");
    MatrixF pts(mesh->get_num_faces(), 3);
    size_t num_pts = pts.rows();
    for (size_t i=0; i<num_pts; i++) {
        pts.row(i) = centroids.segment(i*3, 3);
    }

    locator.locate(pts);
    VectorI elem_indices = locator.get_enclosing_voxels();
    MatrixF barycentric_coords = locator.get_barycentric_coords();

    ASSERT_EQ(num_pts, elem_indices.size());
    ASSERT_EQ(num_pts, barycentric_coords.rows());

    for (size_t i=0; i<num_pts; i++) {
        VectorI voxel = mesh->get_voxel(elem_indices[i]);
        check_barycentric_coord(mesh, pts.row(i), voxel,
                barycentric_coords.row(i));
    }
}

TEST_F(PointLocatorTest, MeshVertices) {
    MeshPtr mesh = load_mesh("tet.msh");
    PointLocator locator(mesh);
    VectorF centroids = mesh->get_vertices();
    MatrixF pts(mesh->get_num_vertices(), 3);
    size_t num_pts = pts.rows();
    for (size_t i=0; i<num_pts; i++) {
        pts.row(i) = centroids.segment(i*3, 3);
    }

    locator.locate(pts);
    VectorI elem_indices = locator.get_enclosing_voxels();
    MatrixF barycentric_coords = locator.get_barycentric_coords();

    ASSERT_EQ(num_pts, elem_indices.size());
    ASSERT_EQ(num_pts, barycentric_coords.rows());

    for (size_t i=0; i<num_pts; i++) {
        VectorI voxel = mesh->get_voxel(elem_indices[i]);
        check_barycentric_coord(mesh, pts.row(i), voxel,
                barycentric_coords.row(i));
    }
}

TEST_F(PointLocatorTest, ZeroElements) {
    MeshPtr mesh = load_mesh("tet.obj");
    ASSERT_THROW(PointLocator locator(mesh), RuntimeError);
}
