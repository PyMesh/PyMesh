/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <set>

#include <Wires/Misc/SymmetryOperators.h>
#include "VectorCompare.h"

class SymmetryOperatorsTest : public ::testing::Test {
    protected:
        typedef SymmetryOperators::Operators Operators;

        template<typename T>
        std::set<T, VectorCompare<T> > gather_orbit(
                const T& seed, const Operators& ops) {
            std::set<T, VectorCompare<T> > orbit;
            orbit.insert(seed);
            size_t orbit_size;
            do {
                orbit_size = orbit.size();
                for (const auto& p : orbit) {
                    for (const auto& op : ops) {
                        orbit.insert(op(p));
                    }
                }
            } while(orbit_size != orbit.size());

            return orbit;
        }
};

TEST_F(SymmetryOperatorsTest, 2D_rotation) {
    Operators ops = SymmetryOperators::generate_rotational_symmetry_operators<2>(
            Vector2F::Zero());
    Vector2F seed(1.0, 0.0);
    auto orbit = gather_orbit(seed, ops);
    ASSERT_EQ(4, orbit.size());
}

TEST_F(SymmetryOperatorsTest, 3D_rotation) {
    Operators ops = SymmetryOperators::generate_rotational_symmetry_operators<3>(
            Vector3F::Zero());
    Vector3F seed(1.0, 0.0, 0.0);
    auto orbit = gather_orbit(seed, ops);
    ASSERT_EQ(6, orbit.size());
}
