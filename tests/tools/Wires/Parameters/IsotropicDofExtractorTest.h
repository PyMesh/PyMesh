/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <WireTest.h>
#include <Wires/Parameters/IsotropicDofExtractor.h>
#include <Wires/WireNetwork/WireNetwork.h>

class IsotropicDofExtractorTest : public WireTest {
    protected:
        void SetUp() {
            m_3d_vertices = MatrixFr(8, 3);
            m_3d_vertices <<
                -1, -1, -1,
                -1, -1,  1,
                -1,  1, -1,
                -1,  1,  1,
                 1, -1, -1,
                 1, -1,  1,
                 1,  1, -1,
                 1,  1,  1;

            m_2d_vertices = MatrixFr(4, 2);
            m_2d_vertices <<
                -1, -1,
                -1,  1,
                 1, -1,
                 1,  1;
        }

    protected:
        MatrixFr m_3d_vertices;
        MatrixIr m_3d_edges;
        MatrixFr m_2d_vertices;
        MatrixIr m_2d_edges;
};

TEST_F(IsotropicDofExtractorTest, 2D_zero_dof) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_2d_vertices, m_2d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector2F> points = {
        Vector2F(-1.0,-1.0),
        Vector2F( 0.0,-1.0),
        Vector2F( 1.0,-1.0),
        Vector2F(-1.0, 0.0),
        Vector2F( 0.0, 0.0),
        Vector2F( 1.0, 0.0),
        Vector2F(-1.0, 1.0),
        Vector2F( 0.0, 1.0),
        Vector2F( 1.0, 1.0)
    };

    for (const auto& p : points) {
        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(0, actual_dofs.size());
    }
}

TEST_F(IsotropicDofExtractorTest, 2D_one_dof) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_2d_vertices, m_2d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector2F> points = {
        Vector2F( 0.0,-0.5),
        Vector2F( 0.1, 0.0),
        Vector2F( 1.0,-0.3),
        Vector2F( 0.9,-1.0),
        Vector2F( 0.5,-0.5),
        Vector2F(-0.1,-0.1)
    };

    std::vector<Vector2F> dofs = {
        Vector2F( 0,-1),
        Vector2F( 1, 0),
        Vector2F( 0,-1),
        Vector2F( 1, 0),
        Vector2F( 1,-1),
        Vector2F(-1,-1)
    };

    const size_t num_points = points.size();
    for (size_t i=0; i<num_points; i++) {
        Vector2F p = points[i];
        Vector2F expected_dof = dofs[i];
        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(1, actual_dofs.size());

        const VectorF& dof = actual_dofs[0];
        ASSERT_FLOAT_EQ(0.0, (dof - expected_dof).norm());
    }
}

TEST_F(IsotropicDofExtractorTest, 2D_two_dofs) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_2d_vertices, m_2d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector2F> points = {
        Vector2F( 0.1,-0.5),
        Vector2F( 0.1, 0.2),
        Vector2F(-0.5,-0.3),
        Vector2F( 0.9,-0.1),
        Vector2F( 0.6,-0.5),
        Vector2F(-0.1,-0.2)
    };

    std::vector<Vector2F> dofs = {
        Vector2F( 1, 0),
        Vector2F( 0,-1),
        Vector2F( 1, 0),
        Vector2F( 0, 1),
        Vector2F(-1, 0),
        Vector2F( 0,-1),
        Vector2F( 1, 0),
        Vector2F( 0,-1),
        Vector2F( 1, 0),
        Vector2F( 0,-1),
        Vector2F(-1, 0),
        Vector2F( 0,-1)
    };

    const size_t num_points = points.size();
    for (size_t i=0; i<num_points; i++) {
        Vector2F p = points[i];
        Vector2F expected_dof_0 = dofs[i*2  ];
        Vector2F expected_dof_1 = dofs[i*2+1];
        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(2, actual_dofs.size());

        for (const auto& dof : actual_dofs) {
            Float dist0 = (dof - expected_dof_0).norm();
            Float dist1 = (dof - expected_dof_1).norm();
            ASSERT_FLOAT_EQ(0.0, std::min(dist0, dist1));
        }
    }
}

TEST_F(IsotropicDofExtractorTest, 3D_zero_dof) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_3d_vertices, m_3d_edges);
    IsotropicDofExtractor extractor(network);

    auto center_dofs = extractor.extract_dofs(Vector3F(0.0, 0.0, 0.0));
    ASSERT_EQ(0, center_dofs.size());

    auto corner_dofs = extractor.extract_dofs(Vector3F(1.0,-1.0, 1.0));
    ASSERT_EQ(0, corner_dofs.size());

    auto face_center_dofs = extractor.extract_dofs(Vector3F(1.0, 0.0, 0.0));
    ASSERT_EQ(0, face_center_dofs.size());

    auto edge_center_dofs = extractor.extract_dofs(Vector3F(1.0, 1.0, 0.0));
    ASSERT_EQ(0, edge_center_dofs.size());
}

TEST_F(IsotropicDofExtractorTest, 3D_one_dof) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_3d_vertices, m_3d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector3F> points = {
        Vector3F( 0.0, 0.0, -0.5),
        Vector3F( 0.1,-0.1,-0.1),
        Vector3F( 0.0,-0.2, 0.2),
        Vector3F( 0.0,-0.5,-1.0),
        Vector3F(-0.1,-0.1, 1.0),
        Vector3F( 0.1, 1.0, 1.0)
    };
    std::vector<Vector3F> dofs = {
        Vector3F( 0, 0,-1),
        Vector3F( 1,-1,-1),
        Vector3F( 0,-1, 1),
        Vector3F( 0,-1, 0),
        Vector3F(-1,-1, 0),
        Vector3F( 1, 0, 0)
    };
    
    const size_t num_samples = points.size();
    for (size_t i=0; i<num_samples; i++) {
        Vector3F p = points[i];
        Vector3F expected_dof = dofs[i];
        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(1, actual_dofs.size());

        const VectorF& dof = actual_dofs[0];
        ASSERT_FLOAT_EQ(0.0, (dof - expected_dof).norm());
    }
}

TEST_F(IsotropicDofExtractorTest, 3D_two_dofs) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_3d_vertices, m_3d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector3F> points = {
        Vector3F( 0.1, -0.1, 0.2),
        Vector3F( 0.0,  0.5, 0.4),
        Vector3F(-0.5,  0.1,-0.1),
        Vector3F(-0.5, -0.1,-1.0)
    };

    std::vector<Vector3F> dofs = {
        Vector3F( 1,-1, 0),
        Vector3F( 0, 0, 1),
        Vector3F( 0, 1, 0),
        Vector3F( 0, 0, 1),
        Vector3F(-1, 0, 0),
        Vector3F( 0, 1,-1),
        Vector3F(-1, 0, 0),
        Vector3F( 0,-1, 0)
    };

    const size_t num_samples = points.size();
    for (size_t i=0; i<num_samples; i++) {
        Vector3F p = points[i];
        Vector3F expected_dof_0 = dofs[i*2  ];
        Vector3F expected_dof_1 = dofs[i*2+1];

        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(2, actual_dofs.size());

        for (auto& dof : actual_dofs) {
            Float dist0 = (expected_dof_0 - dof).norm();
            Float dist1 = (expected_dof_1 - dof).norm();
            ASSERT_FLOAT_EQ(0.0, std::min(dist0, dist1));
        }
    }
}

TEST_F(IsotropicDofExtractorTest, 3D_three_dofs) {
    WireNetwork::Ptr network = WireNetwork::create_raw(m_3d_vertices, m_3d_edges);
    IsotropicDofExtractor extractor(network);

    std::vector<Vector3F> points = {
        Vector3F( 0.1, -0.2, -0.3),
        Vector3F(-0.9,  0.1, -0.5)
    };
    std::vector<Vector3F> dofs = {
        Vector3F( 1, 0, 0),
        Vector3F( 0,-1, 0),
        Vector3F( 0, 0,-1),
        Vector3F(-1, 0, 0),
        Vector3F( 0, 1, 0),
        Vector3F( 0, 0,-1)
    };

    const size_t num_samples = points.size();
    for (size_t i=0; i<num_samples; i++) {
        Vector3F p = points[i];
        Vector3F expected_dof_0 = dofs[i*3  ];
        Vector3F expected_dof_1 = dofs[i*3+1];
        Vector3F expected_dof_2 = dofs[i*3+2];

        auto actual_dofs = extractor.extract_dofs(p);
        ASSERT_EQ(3, actual_dofs.size());

        for (auto& dof : actual_dofs) {
            Float dist0 = (expected_dof_0 - dof).norm();
            Float dist1 = (expected_dof_1 - dof).norm();
            Float dist2 = (expected_dof_2 - dof).norm();
            ASSERT_FLOAT_EQ(0.0, std::min({dist0, dist1, dist2}));
        }
    }
}

