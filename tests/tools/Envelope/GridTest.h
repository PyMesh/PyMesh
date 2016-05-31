/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>
#include <Envelope/Grid.h>

#include <TestBase.h>

class GridTest : public TestBase {
    protected:
        typedef Grid<2, short> ShortGrid2D;
        typedef Grid<3, short> ShortGrid3D;
        typedef Grid<2, bool> BoolGrid2D;
        typedef Grid<3, bool> BoolGrid3D;

        template<int DIM, typename T>
        Grid<DIM, T> init() {
            typedef typename Grid<DIM, T>::Vector_i Vector_i;
            typedef typename Grid<DIM, T>::Vector_f Vector_f;
            Grid<DIM, T> grid(0.5);
            Vector_i size = Vector_i::Ones() * 2;
            Vector_f base_coord = Vector_f::Zero();
            grid.initialize(size, base_coord);

            return grid;
        }
};

TEST_F(GridTest, 2D) {
    ShortGrid2D grid = init<2, short>();
    Vector2I size = grid.size();

    for (size_t i=0; i<size[0]; i++) {
        for (size_t j=0; j<size[1]; j++) {
            ASSERT_EQ(0, grid(Vector2I(i,j)));
        }
    }
}

TEST_F(GridTest, 3D) {
    ShortGrid3D grid = init<3, short>();
    Vector3I size = grid.size();

    for (size_t i=0; i<size[0]; i++) {
        for (size_t j=0; j<size[1]; j++) {
            for (size_t k=0; k<size[2]; k++) {
                ASSERT_EQ(0, grid(Vector3I(i,j,k)));
            }
        }
    }
}

TEST_F(GridTest, Assignment2D) {
    ShortGrid2D grid = init<2, short>();

    grid.lookup(grid.base_coordinates()) = 1;
    ASSERT_EQ(1, grid.lookup(grid.base_coordinates()));
    ASSERT_EQ(0, grid.lookup(grid.base_coordinates() + grid.cell_size()));
}

TEST_F(GridTest, Assignment3D) {
    ShortGrid3D grid = init<3, short>();

    grid.lookup(grid.base_coordinates()) = 1;
    ASSERT_EQ(1, grid.lookup(grid.base_coordinates()));
    ASSERT_EQ(0, grid.lookup(grid.base_coordinates() + grid.cell_size()));
}

TEST_F(GridTest, GridBound2D) {
    const Float EPS = 1e-6;
    ShortGrid2D grid = init<2, short>();
    Vector2F p = grid.base_coordinates();
    Vector2F step = grid.cell_size();
    Vector2I size = grid.size();
    Vector2F bbox_size = step.cwiseProduct(size.template cast<Float>());

    Vector2F center = p + (bbox_size - step) * 0.5;
    ASSERT_TRUE(grid.is_inside(center));
    // Corners would be exactly on the boundary of the boundary cells.
    // Inside/outside depends on if the coordinates are positive/negative.
    Vector2F corners[4] = {
        center + Vector2F( bbox_size[0], bbox_size[1]) * 0.5,
        center + Vector2F( bbox_size[0],-bbox_size[1]) * 0.5,
        center + Vector2F(-bbox_size[0],-bbox_size[1]) * 0.5,
        center + Vector2F(-bbox_size[0], bbox_size[1]) * 0.5 };
    Vector2F epsilon[4] = {
        Vector2F( EPS, EPS),
        Vector2F( EPS,-EPS),
        Vector2F(-EPS,-EPS),
        Vector2F(-EPS, EPS) };

    for (size_t i=0; i<4; i++) {
        ASSERT_FALSE(grid.is_inside(corners[i] + epsilon[i]));
        ASSERT_TRUE(grid.is_inside(corners[i] - epsilon[i]));
    }
}

TEST_F(GridTest, GridBound3D) {
    const Float EPS = 1e-6;
    ShortGrid3D grid = init<3, short>();
    Vector3F p = grid.base_coordinates();
    Vector3F step = grid.cell_size();
    Vector3I size = grid.size();
    Vector3F bbox_size = step.cwiseProduct(size.template cast<Float>());

    Vector3F center = p + (bbox_size - step) * 0.5;
    ASSERT_TRUE(grid.is_inside(center));
    // Corners would be exactly on the boundary of the boundary cells.
    // Inside/outside depends on if the coordinates are positive/negative.
    Vector3F corners[8] = {
        center + Vector3F( bbox_size[0], bbox_size[1], bbox_size[2]) * 0.5,
        center + Vector3F( bbox_size[0], bbox_size[1],-bbox_size[2]) * 0.5,
        center + Vector3F( bbox_size[0],-bbox_size[1],-bbox_size[2]) * 0.5,
        center + Vector3F( bbox_size[0],-bbox_size[1], bbox_size[2]) * 0.5,
        center + Vector3F(-bbox_size[0], bbox_size[1], bbox_size[2]) * 0.5,
        center + Vector3F(-bbox_size[0], bbox_size[1],-bbox_size[2]) * 0.5,
        center + Vector3F(-bbox_size[0],-bbox_size[1],-bbox_size[2]) * 0.5,
        center + Vector3F(-bbox_size[0],-bbox_size[1], bbox_size[2]) * 0.5 };
    Vector3F epsilon[8] = {
        Vector3F( EPS, EPS, EPS),
        Vector3F( EPS, EPS,-EPS),
        Vector3F( EPS,-EPS,-EPS),
        Vector3F( EPS,-EPS, EPS),
        Vector3F(-EPS, EPS, EPS),
        Vector3F(-EPS, EPS,-EPS),
        Vector3F(-EPS,-EPS,-EPS),
        Vector3F(-EPS,-EPS, EPS) };

    for (size_t i=0; i<8; i++) {
        ASSERT_FALSE(grid.is_inside(corners[i] + epsilon[i]));
        ASSERT_TRUE(grid.is_inside(corners[i] - epsilon[i]));
    }
}

