/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#pragma once
#ifdef WITH_GEOGRAM
#include <string>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Geogram/GeogramMeshUtils.h>

#include <TestBase.h>

class GeogramMeshUtilsTest : public TestBase {
};

TEST_F(GeogramMeshUtilsTest, wire_network) {
    MatrixFr vertices(3, 3);
    vertices << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0;
    Matrix2Ir edges(3, 2);
    edges << 0, 1,
             1, 2,
             2, 0;
    auto mesh = GeogramMeshUtils::wire_network_to_geomesh(vertices, edges);

    ASSERT_EQ(3, mesh->vertices.nb());
    ASSERT_EQ(0, mesh->facets.nb());
    ASSERT_EQ(3, mesh->edges.nb());
}


#endif
