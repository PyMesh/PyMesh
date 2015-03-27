/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Wires/Misc/SymmetryChecker.h>
#include <Wires/Parameters/ParameterManager.h>
#include <Wires/Parameters/ParameterCommon.h>

#include <WireTest.h>

class SymmetryCheckerTest : public WireTest {
    protected:
        /**
         * Apply isotropic offset to the vertices of wire_network;
         */
        void apply_isotropic_offset(WireNetwork::Ptr wire_network) {
            auto manager = ParameterManager::create_isotropic(
                    wire_network, 0.5, ParameterCommon::VERTEX);

            const size_t num_dofs = manager->get_num_dofs();
            VectorF dofs = manager->get_dofs();
            dofs += MatrixUtils::range<VectorF>(num_dofs) * 0.01;
            manager->set_dofs(dofs);

            auto offset = manager->evaluate_offset_no_formula();
            wire_network->set_vertices(
                    wire_network->get_vertices() + offset);
        }
};

TEST_F(SymmetryCheckerTest, cube) {
    const auto wire_network = load_wire_shared("cube.wire");
    SymmetryChecker checker(wire_network);
    ASSERT_TRUE(checker.has_cubic_symmetry());

    apply_isotropic_offset(wire_network);

    checker.update();
    ASSERT_TRUE(checker.has_cubic_symmetry());
}

TEST_F(SymmetryCheckerTest, diamond) {
    const auto wire_network = load_wire_shared("diamond.wire");
    SymmetryChecker checker(wire_network);
    ASSERT_FALSE(checker.has_cubic_symmetry());
}

TEST_F(SymmetryCheckerTest, pattern1065) {
    const auto wire_network = load_wire_shared("pattern1065.wire");
    SymmetryChecker checker(wire_network);
    ASSERT_TRUE(checker.has_cubic_symmetry());

    apply_isotropic_offset(wire_network);

    checker.update();
    ASSERT_TRUE(checker.has_cubic_symmetry());
}
