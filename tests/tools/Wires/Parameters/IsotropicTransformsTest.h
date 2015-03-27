/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <WireTest.h>
#include <Wires/Parameters/IsotropicTransforms.h>
#include <Wires/WireNetwork/WireNetwork.h>

class IsotropicTransformsTest : public WireTest {
    protected:
        void ASSERT_ISOTROPIC(const VectorF& from, const VectorF& to) {
            const size_t dim = from.size();
            IsotropicTransforms trans(dim);
            MatrixF M = trans.fit(from, to);
            ASSERT_FLOAT_EQ(0.0, (M * from - to).norm());
        }
};

TEST_F(IsotropicTransformsTest, 2D_mid_edge) {
    Vector2F from(1.0, 0.0);
    std::vector<Vector2F> to = {
        Vector2F( 1.0, 0.0),
        Vector2F(-1.0, 0.0),
        Vector2F( 0.0, 1.0),
        Vector2F( 0.0,-1.0)
    };

    for (const auto& v : to) {
        ASSERT_ISOTROPIC(from, v);
    }
}

TEST_F(IsotropicTransformsTest, 2D_on_edge) {
    Vector2F from(1.0, 0.1);
    std::vector<Vector2F> to = {
        Vector2F( 1.0, 0.1),
        Vector2F(-1.0, 0.1),
        Vector2F( 1.0,-0.1),
        Vector2F(-1.0,-0.1),
        Vector2F( 0.1, 1.0),
        Vector2F( 0.1,-1.0),
        Vector2F(-0.1, 1.0),
        Vector2F(-0.1,-1.0)
    };

    for (const auto& v : to) {
        ASSERT_ISOTROPIC(from, v);
    }
}

TEST_F(IsotropicTransformsTest, mid_face) {
    Vector3F from(1, 0, 0);
    std::vector<Vector3F> to = {
        Vector3F( 1,  0,  0),
        Vector3F(-1,  0,  0),
        Vector3F( 0,  1,  0),
        Vector3F( 0, -1,  0),
        Vector3F( 0,  0,  1),
        Vector3F( 0,  0, -1)
    };

    for (const auto& v : to) {
        ASSERT_ISOTROPIC(from, v);
    }
}

TEST_F(IsotropicTransformsTest, mid_edge) {
    Vector3F from(1, 1, 0);
    std::vector<Vector3F> to = {
        Vector3F( 1,  1,  0),
        Vector3F(-1,  1,  0),
        Vector3F( 1, -1,  0),
        Vector3F(-1, -1,  0),
        Vector3F( 0,  1,  1),
        Vector3F( 0, -1,  1),
        Vector3F( 0,  1, -1),
        Vector3F( 0, -1, -1),
        Vector3F( 1,  0,  1),
        Vector3F( 1,  0, -1),
        Vector3F(-1,  0,  1),
        Vector3F(-1,  0, -1)
    };

    for (const auto& v : to) {
        ASSERT_ISOTROPIC(from, v);
    }
}

TEST_F(IsotropicTransformsTest, on_face) {
    Vector3F from(1, 0.1, 0.1);
    std::vector<Vector3F> to = {
        Vector3F( 1, 0.1, 0.1),
        Vector3F(-1, 0.1, 0.1),
        Vector3F( 1,-0.1, 0.1),
        Vector3F(-1,-0.1, 0.1),
        Vector3F( 1, 0.1,-0.1),
        Vector3F(-1, 0.1,-0.1),
        Vector3F( 1,-0.1,-0.1),
        Vector3F(-1,-0.1,-0.1),

        Vector3F( 0.1, 1, 0.1),
        Vector3F( 0.1,-1, 0.1),
        Vector3F(-0.1, 1, 0.1),
        Vector3F(-0.1,-1, 0.1),
        Vector3F( 0.1, 1,-0.1),
        Vector3F( 0.1,-1,-0.1),
        Vector3F(-0.1, 1,-0.1),
        Vector3F(-0.1,-1,-0.1),

        Vector3F( 0.1, 0.1, 1),
        Vector3F( 0.1, 0.1,-1),
        Vector3F(-0.1, 0.1, 1),
        Vector3F(-0.1, 0.1,-1),
        Vector3F( 0.1,-0.1, 1),
        Vector3F( 0.1,-0.1,-1),
        Vector3F(-0.1,-0.1, 1),
        Vector3F(-0.1,-0.1,-1)
    };

    for (const auto& v : to) {
        ASSERT_ISOTROPIC(from, v);
    }
}

