/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cmath>
#include <TestBase.h>

class EdgeDihedralAngleAttributeTest : public TestBase {
    protected:
};

TEST_F(EdgeDihedralAngleAttributeTest, tet) {
    MeshPtr mesh = load_mesh("tet.msh");
    mesh->add_attribute("edge_dihedral_angle");
    ASSERT_TRUE(mesh->has_attribute("edge_dihedral_angle"));
    VectorF dihedral_angles = mesh->get_attribute("edge_dihedral_angle");
    ASSERT_FLOAT_EQ(dihedral_angles.sum() / dihedral_angles.size(),
            dihedral_angles[0]);
    ASSERT_FLOAT_EQ(M_PI - atan(2*sqrt(2)), dihedral_angles[0]);
}

TEST_F(EdgeDihedralAngleAttributeTest, hex) {
    MeshPtr mesh = load_mesh("hex.msh");
    mesh->add_attribute("edge_dihedral_angle");
    ASSERT_TRUE(mesh->has_attribute("edge_dihedral_angle"));
    VectorF dihedral_angles = mesh->get_attribute("edge_dihedral_angle");
    ASSERT_FLOAT_EQ(dihedral_angles.sum() / dihedral_angles.size(),
            dihedral_angles[0]);
    ASSERT_FLOAT_EQ(M_PI/2.0, dihedral_angles[0]);
}
