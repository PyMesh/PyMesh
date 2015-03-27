/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Assembler/Materials/UniformMaterial.h>
#include <Assembler/Materials/PeriodicMaterial.h>

#include "MaterialTest.h"

class PeriodicMaterialTest : public MaterialTest {
    protected:
        void init_material_tensor(size_t dim, Float val) {
            size_t tensor_size = dim*dim;
            m_material_tensor = MatrixF(tensor_size, tensor_size);
            for (size_t i=0; i<tensor_size; i++) {
                for (size_t j=0; j<tensor_size; j++) {
                    m_material_tensor(i,j) = val;
                }
            }
        }

        MaterialPtr create_uniform(size_t dim, Float val) {
            if (dim == 2 || dim == 3) {
                init_material_tensor(dim, val);
            } else {
                throw RuntimeError("Unknown dim.");
            }
            return MaterialPtr(new UniformMaterial(
                        m_density, m_material_tensor));
        }

        MaterialPtr create_periodic(MaterialPtr mat1, MaterialPtr mat2,
                VectorF axis, Float period, Float ratio, Float phase=0.0) {
            return MaterialPtr(new PeriodicMaterial(mat1, mat2, axis,
                        period, ratio, phase));
        }

    protected:
        MatrixF m_material_tensor;
};

TEST_F(PeriodicMaterialTest, 2DRank1) {
    Vector2F axis_1(1, 0);
    Float period_1 = 2.0;
    Float ratio_1 = 0.5;

    MaterialPtr mat1 = create_uniform(2, 1.0);
    MaterialPtr mat2 = create_uniform(2, 2.0);

    MaterialPtr laminate = create_periodic(mat1, mat2, axis_1, period_1, ratio_1);

    Vector2F sample_1 = axis_1 * period_1 * ratio_1 * 0.5;
    Vector2F sample_2 = axis_1 * period_1 * (ratio_1 + (1.0 - ratio_1) * 0.5);

    assert_material_eq(mat1, laminate, 2, sample_1);
    assert_material_eq(mat2, laminate, 2, sample_2);
}

TEST_F(PeriodicMaterialTest, Rank1Phase) {
    Vector2F axis_1(1, 0);
    Float period_1 = 2.0;
    Float ratio_1 = 0.5;
    Float phase_1 = 0.25;

    MaterialPtr mat1 = create_uniform(2, 1.0);
    MaterialPtr mat2 = create_uniform(2, 2.0);

    MaterialPtr laminate = create_periodic(mat1, mat2, axis_1,
            period_1, ratio_1, phase_1);

    Vector2F sample_1(0.0, 0.0);
    Vector2F sample_2 = axis_1 * period_1 * (phase_1 + 0.5 * ratio_1);

    assert_material_eq(mat2, laminate, 2, sample_1);
    assert_material_eq(mat1, laminate, 2, sample_2);
}

TEST_F(PeriodicMaterialTest, 2DRank2) {
    // This test would only work for orthognal lamination directions.
    Vector2F axis_1(1.0, 0.0);
    Vector2F axis_2(0.0, 1.0);

    Float period_1 = 2.0;
    Float period_2 = 1.0;
    Float ratio_1 = 0.5;
    Float ratio_2 = 0.75;

    MaterialPtr mat1 = create_uniform(2, 1.0);
    MaterialPtr mat2 = create_uniform(2, 2.0);

    MaterialPtr laminate_1 = create_periodic(mat1, mat2, axis_1, period_1, ratio_1);
    MaterialPtr laminate_2 = create_periodic(laminate_1, mat2, axis_2, period_2, ratio_2);

    Vector2F mid_point = axis_1 * period_1 * ratio_1 + axis_2 * period_2 * ratio_2;
    Vector2F sample_1 = mid_point * 0.5;
    Vector2F sample_2 = mid_point + axis_1 * period_1 * (1 - ratio_1) * 0.5;
    Vector2F sample_3 = mid_point + axis_2 * period_2 * (1 - ratio_2) * 0.5;
    Vector2F sample_4 = mid_point + axis_1 * period_1 * (1 - ratio_1) * 0.5
        + axis_2 * period_2 * (1 - ratio_2) * 0.5;

    assert_material_eq(mat1, laminate_2, 2, sample_1);
    assert_material_eq(mat2, laminate_2, 2, sample_2);
    assert_material_eq(mat2, laminate_2, 2, sample_3);
    assert_material_eq(mat2, laminate_2, 2, sample_4);
}

TEST_F(PeriodicMaterialTest, 3DRank3) {
    // Note the non-orthogonal frame.
    Vector3F axis_1(1.0, 0.0, 0.0);
    Vector3F axis_2(1.0, 1.0, 0.0);
    Vector3F axis_3(0.0, 0.0, 0.5);

    Float period_1 = 3.0;
    Float period_2 = 2.0;
    Float period_3 = 1.0;

    Float ratio_1 = 0.5;
    Float ratio_2 = 0.75;
    Float ratio_3 = 0.1;

    MaterialPtr mat1 = create_uniform(3, 1.0);
    MaterialPtr mat2 = create_uniform(3, 2.0);

    MaterialPtr laminate_1 = create_periodic(mat1      , mat2, axis_1, period_1, ratio_1);
    MaterialPtr laminate_2 = create_periodic(laminate_1, mat2, axis_2, period_2, ratio_2);
    MaterialPtr laminate_3 = create_periodic(laminate_2, mat2, axis_3, period_3, ratio_3);

    Vector3F base_1 = axis_1 * period_1 * ratio_1;
    Vector3F base_2 = axis_2 * period_2 * ratio_2;
    Vector3F base_3 = axis_3 * period_3 * ratio_3;

    Vector3F inc_1 = axis_1 * period_1 * (1.0 - ratio_1) * 0.5;
    Vector3F inc_2 = axis_2 * period_2 * (1.0 - ratio_2) * 0.5;
    Vector3F inc_3 = axis_3 * period_3 * (1.0 - ratio_3) * 0.5;

    assert_material_eq(mat1, laminate_3, 3, m_origin);
    assert_material_eq(mat2, laminate_3, 3, base_1 + inc_1);
    assert_material_eq(mat2, laminate_3, 3, base_2 + inc_2);
    assert_material_eq(mat2, laminate_3, 3, base_3 + inc_3);
}

