/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <iostream>
#include <Misc/HashGrid.h>
#include <Misc/Timer.h>
#include <Core/EigenTypedef.h>

using namespace PyMesh;

MatrixFr generate_evenly_spaced(size_t resolution) {
    MatrixFr points(resolution*resolution*resolution, 3);
    for (size_t i=0; i<resolution; i++) {
        for (size_t j=0; j<resolution; j++) {
            for (size_t k=0; k<resolution; k++) {
                points.row(k + resolution * (j + resolution*i)) = Vector3F(
                        Float(i) / Float(resolution-1),
                        Float(j) / Float(resolution-1),
                        Float(k) / Float(resolution-1));
            }
        }
    }
    return points;
}

void test_std_hash(const MatrixFr& points, Float cell_size) {
    const size_t num_pts = points.rows();

    Timer timer("Standard hash");
    HashGrid::Ptr grid = HashGrid::create(cell_size, 3, HashGrid::STL_HASH);
    timer.tik("creation");
    for (size_t i=0; i<num_pts; i++) {
        grid->insert(i, points.row(i));
    }
    timer.tik("insertion");

    timer.summary();
}

void test_dense_hash(const MatrixFr& points, Float cell_size) {
    const size_t num_pts = points.rows();

    Timer timer("Google dense hash");
    HashGrid::Ptr grid = HashGrid::create(cell_size, 3, HashGrid::DENSE_HASH);
    timer.tik("creation");
    for (size_t i=0; i<num_pts; i++) {
        grid->insert(i, points.row(i));
    }
    timer.tik("insertion");

    timer.summary();
}

void test_sparse_hash(const MatrixFr& points, Float cell_size) {
    const size_t num_pts = points.rows();

    Timer timer("Google sparse hash");
    HashGrid::Ptr grid = HashGrid::create(cell_size, 3, HashGrid::SPARSE_HASH);
    timer.tik("creation");
    for (size_t i=0; i<num_pts; i++) {
        grid->insert(i, points.row(i));
    }
    timer.tik("insertion");

    timer.summary();
}

int main() {
    const size_t resolution = 200;
    Float cell_size = 0.1;
    MatrixFr points = generate_evenly_spaced(resolution);
    test_std_hash(points, cell_size);
    test_sparse_hash(points, cell_size);
    test_dense_hash(points, cell_size);
    return 0;
}
