/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once

#include <TestBase.h>
#include <BVH/BVHEngine.h>

#include <Mesh.h>

class BVHTest : public TestBase {
    public:
        void init_bvh(MeshPtr mesh, typename BVHEngine::Ptr bvh) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();
            const size_t num_faces = mesh->get_num_faces();

            VectorF flattened_vertices = mesh->get_vertices();
            MatrixFr vertices(num_vertices, dim);
            std::copy( flattened_vertices.data(),
                    flattened_vertices.data() + num_vertices * dim,
                    vertices.data());

            VectorI flattened_faces = mesh->get_faces();
            MatrixIr faces(num_faces, 3);
            std::copy( flattened_faces.data(),
                    flattened_faces.data() + num_faces* 3,
                    faces.data());

            bvh->set_mesh(vertices, faces);
            bvh->build();
        }

        void assert_centroid_has_zero_dist(MeshPtr mesh,
                typename BVHEngine::Ptr bvh,
                bool reordered=false,
                const Float tol=1.0e-12) {
            const size_t dim = mesh->get_dim();
            const size_t num_faces = mesh->get_num_faces();

            mesh->add_attribute("face_centroid");
            VectorF flattened_centroids = mesh->get_attribute("face_centroid");
            MatrixFr centroids(num_faces, dim);
            std::copy(flattened_centroids.data(),
                    flattened_centroids.data() + num_faces * dim,
                    centroids.data());

            VectorF distances;
            VectorI face_indices;
            MatrixFr closest_points;
            bvh->lookup(centroids, distances, face_indices, closest_points);

            ASSERT_EQ(num_faces, distances.size());
            ASSERT_EQ(num_faces, face_indices.size());
            ASSERT_EQ(num_faces, closest_points.rows());

            for (size_t i=0; i<num_faces; i++) {
                if (!reordered) {
                    ASSERT_EQ(i, face_indices[i]);
                }
                ASSERT_NEAR(0.0,
                        (centroids.row(i) - closest_points.row(i)).squaredNorm(), tol);
                ASSERT_NEAR(0.0, distances[i], tol);
            }
        }

        void assert_vertex_has_zero_dist(MeshPtr mesh,
                typename BVHEngine::Ptr bvh,
                bool reordered=false,
                const Float tol=1.0e-12) {
            const size_t dim = mesh->get_dim();
            const size_t num_vertices = mesh->get_num_vertices();

            VectorF flattened_vertices = mesh->get_vertices();
            MatrixFr vertices(num_vertices, dim);
            std::copy( flattened_vertices.data(),
                    flattened_vertices.data() + num_vertices * dim,
                    vertices.data());

            VectorF distances;
            VectorI face_indices;
            MatrixFr closest_points;
            bvh->lookup(vertices, distances, face_indices, closest_points);

            ASSERT_EQ(num_vertices, distances.size());
            ASSERT_EQ(num_vertices, face_indices.size());
            ASSERT_EQ(num_vertices, closest_points.rows());

            for (size_t i=0; i<num_vertices; i++) {
                const auto f = mesh->get_face(face_indices[i]);
                if (!reordered) {
                    ASSERT_TRUE(f[0] == i || f[1] == i || f[2] == i);
                }
                ASSERT_NEAR(0.0,
                        (vertices.row(i) - closest_points.row(i)).squaredNorm(), tol);
                ASSERT_NEAR(0.0, distances[i], tol);
            }
        }

        void simple_triangle_test(typename BVHEngine::Ptr bvh) {
            MatrixFr vertices(3, 3);
            vertices << 0.0, 0.0, 0.0,
                     1.0, 0.0, 0.0,
                     0.0, 1.0, 0.0;
            MatrixIr faces(1, 3);
            faces << 0, 1, 2;

            bvh->set_mesh(vertices, faces);
            bvh->build();

            MatrixFr queries(3, 3);
            queries << -1.0, 0.0, 0.0, // Coplanar but outside.
                        0.5, 0.0, 0.0, // On edge.
                        0.5, 0.5, 1.0; // Outside but closest point on edge.

            VectorF distances;
            VectorI face_indices;
            MatrixFr closest_points;
            bvh->lookup(queries, distances, face_indices, closest_points);

            ASSERT_EQ(face_indices[0], 0);
            ASSERT_EQ(face_indices[1], 0);
            ASSERT_EQ(face_indices[2], 0);

            ASSERT_FLOAT_EQ(1.0, distances[0]);
            ASSERT_FLOAT_EQ(0.0, distances[1]);
            ASSERT_FLOAT_EQ(1.0, distances[2]);

            ASSERT_FLOAT_EQ(0.0, closest_points(0, 0));
            ASSERT_FLOAT_EQ(0.0, closest_points(0, 1));
            ASSERT_FLOAT_EQ(0.0, closest_points(0, 2));

            ASSERT_FLOAT_EQ(0.5, closest_points(1, 0));
            ASSERT_FLOAT_EQ(0.0, closest_points(1, 1));
            ASSERT_FLOAT_EQ(0.0, closest_points(1, 2));

            ASSERT_FLOAT_EQ(0.5, closest_points(2, 0));
            ASSERT_FLOAT_EQ(0.5, closest_points(2, 1));
            ASSERT_FLOAT_EQ(0.0, closest_points(2, 2));
        }

        void hinge_test(typename BVHEngine::Ptr bvh, bool reordered=false,
                const Float tol=1e-12) {
            MatrixFr vertices(6, 3);
            vertices << 0.0, 0.0, 0.0,
                     0.0, 0.0, 1.0,
                     -1.0, 0.0, 0.0,
                     0.0, -1.0, 0.0,
                     -1.0, 0.0, 1.0,
                     0.0, -1.0, 1.0;

            MatrixIr faces(4, 3);
            faces << 0, 1, 2,
                     1, 0, 3,
                     1, 4, 2,
                     1, 3, 5;

            bvh->set_mesh(vertices, faces);
            bvh->build();

            const size_t N = 100;
            MatrixFr queries(N, 3);
            for (size_t i=0; i<N; i++) {
                Float angle = 2*M_PI * Float(i) / Float(N);
                queries(i,0) = cos(angle);
                queries(i,1) = sin(angle);
                queries(i,2) = 0.5;
            }

            VectorF distances;
            VectorI face_indices;
            MatrixFr closest_points;
            bvh->lookup(queries, distances, face_indices, closest_points);

            for (size_t i=0; i<N; i++) {
                const auto& p = queries.row(i);
                if (p[0] >= 0.0 && p[1] >= 0.0) {
                    // 1st quadrant.
                    ASSERT_NEAR(1.0, distances[i], tol);
                    if (!reordered) {
                        ASSERT_TRUE(face_indices[i] == 0 || face_indices[i] == 1);
                    }
                    ASSERT_NEAR(0.0, closest_points(i, 0), tol);
                    ASSERT_NEAR(0.0, closest_points(i, 1), tol);
                    ASSERT_NEAR(0.5, closest_points(i, 2), tol);
                } else if (p[1] >= 0.0) {
                    // 2nd quadrant.
                    ASSERT_NEAR(p[1]*p[1], distances[i], tol);
                    if (!reordered) {
                        if (std::abs(p[0]) < 0.5) {
                            ASSERT_EQ(0, face_indices[i]);
                        } else if (std::abs(p[0]) > 0.5) {
                            ASSERT_EQ(2, face_indices[i]);
                        } else {
                            ASSERT_TRUE(face_indices[i] == 0 ||
                                    face_indices[i] == 2);
                        }
                    }
                    ASSERT_NEAR(p[0], closest_points(i, 0), tol);
                    ASSERT_NEAR(0.0, closest_points(i, 1), tol);
                    ASSERT_NEAR(0.5, closest_points(i, 2), tol);
                } else if (p[0] >= 0.0) {
                    // 4th quadrant.
                    ASSERT_NEAR(p[0]*p[0], distances[i], tol);
                    if (!reordered) {
                        if (std::abs(p[1]) < 0.5) {
                            ASSERT_EQ(1, face_indices[i]);
                        } else if (std::abs(p[1]) > 0.5) {
                            ASSERT_EQ(3, face_indices[i]);
                        } else {
                            ASSERT_TRUE(face_indices[i] == 1 || face_indices[i] == 3);
                        }
                    }
                    ASSERT_NEAR(0.0, closest_points(i, 0), tol);
                    ASSERT_NEAR(p[1], closest_points(i, 1), tol);
                    ASSERT_NEAR(0.5, closest_points(i, 2), tol);
                } else {
                    // 3rd quadrant.
                    if (std::abs(p[0]) > std::abs(p[1])) {
                        ASSERT_NEAR(p[1]*p[1], distances[i], tol);
                        if (!reordered) {
                            ASSERT_EQ(2, face_indices[i]);
                        }
                        ASSERT_NEAR(p[0], closest_points(i, 0), tol);
                        ASSERT_NEAR(0.0, closest_points(i, 1), tol);
                        ASSERT_NEAR(0.5, closest_points(i, 2), tol);
                    } else if (std::abs(p[0]) < std::abs(p[1])) {
                        ASSERT_NEAR(p[0]*p[0], distances[i], tol);
                        if (!reordered) {
                            ASSERT_EQ(3, face_indices[i]);
                        }
                        ASSERT_NEAR(0.0, closest_points(i, 0), tol);
                        ASSERT_NEAR(p[1], closest_points(i, 1), tol);
                        ASSERT_NEAR(0.5, closest_points(i, 2), tol);
                    } else {
                        if (!reordered) {
                            ASSERT_TRUE(face_indices[i] == 2 ||
                                    face_indices[i] == 3);
                        }
                        ASSERT_NEAR(p[0]*p[0], distances[i], tol);
                    }
                }
            }
        }
};

#if WITH_CGAL
TEST_F(BVHTest, cgal_aabb) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto bvh = BVHEngine::create("cgal", 3);
    ASSERT_TRUE(bool(bvh));
    init_bvh(mesh, bvh);
    assert_centroid_has_zero_dist(mesh, bvh);
    assert_vertex_has_zero_dist(mesh, bvh);

    // Ensure resetting works.
    MeshPtr mesh2 = load_mesh("ball.msh");
    init_bvh(mesh2, bvh);
    assert_centroid_has_zero_dist(mesh2, bvh);
    assert_vertex_has_zero_dist(mesh2, bvh);
}

TEST_F(BVHTest, cgal_simple) {
    auto bvh = BVHEngine::create("cgal", 3);
    simple_triangle_test(bvh);
}

TEST_F(BVHTest, cgal_hinge) {
    auto bvh = BVHEngine::create("cgal", 3);
    hinge_test(bvh);
}
#endif

#if WITH_GEOGRAM
TEST_F(BVHTest, geogram_aabb) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto bvh = BVHEngine::create("geogram", 3);
    ASSERT_TRUE(bool(bvh));
    init_bvh(mesh, bvh);
    assert_centroid_has_zero_dist(mesh, bvh, true);
    assert_vertex_has_zero_dist(mesh, bvh, true);

    // Ensure resetting works.
    MeshPtr mesh2 = load_mesh("ball.msh");
    init_bvh(mesh2, bvh);
    assert_centroid_has_zero_dist(mesh2, bvh, true);
    assert_vertex_has_zero_dist(mesh2, bvh, true);
}

TEST_F(BVHTest, geogram_simple) {
    auto bvh = BVHEngine::create("geogram", 3);
    simple_triangle_test(bvh);
}

TEST_F(BVHTest, geogram_hinge) {
    auto bvh = BVHEngine::create("geogram", 3);
    hinge_test(bvh);
}
#endif

#if WITH_IGL
TEST_F(BVHTest, igl_aabb) {
    MeshPtr mesh = load_mesh("cube.obj");
    auto bvh = BVHEngine::create("igl", 3);
    ASSERT_TRUE(bool(bvh));
    init_bvh(mesh, bvh);
    assert_centroid_has_zero_dist(mesh, bvh, true);
    assert_vertex_has_zero_dist(mesh, bvh, true);

    // Ensure resetting works.
    MeshPtr mesh2 = load_mesh("ball.msh");
    init_bvh(mesh2, bvh);
    assert_centroid_has_zero_dist(mesh2, bvh, true);
    assert_vertex_has_zero_dist(mesh2, bvh, true);
}

TEST_F(BVHTest, igl_aabb_2D) {
    MeshPtr mesh = load_mesh("square_2D.obj");
    auto bvh = BVHEngine::create("igl", 2);
    ASSERT_TRUE(bool(bvh));
    init_bvh(mesh, bvh);
    assert_centroid_has_zero_dist(mesh, bvh, true);
    assert_vertex_has_zero_dist(mesh, bvh, true);
}

TEST_F(BVHTest, igl_simple) {
    auto bvh = BVHEngine::create("igl", 3);
    simple_triangle_test(bvh);
}

TEST_F(BVHTest, igl_hinge) {
    auto bvh = BVHEngine::create("igl", 3);
    hinge_test(bvh);
}
#endif


