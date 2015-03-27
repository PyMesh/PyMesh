/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Attributes/WireAttribute.h>
#include <WireTest.h>

class WireAttributeTest : public WireTest {
};

TEST_F(WireAttributeTest, Creation) {
    MatrixFr values(1, 1);
    values.coeffRef(0, 0) = 1.0;

    WireAttribute attr;
    attr.set_values(values);

    ASSERT_EQ(1, attr.size());
    ASSERT_FLOAT_EQ(0.0, (attr.get_values() - values).norm());
}
