#include "Classes.h"

void SparseMat::initialize(
        VectorI& inner_indices, VectorI& outer_indices, VectorF& values) {
    for (size_t col=0; col < cols(); col++) {
        const size_t start_idx = outer_indices[col];
        const size_t end_idx = outer_indices[col+1];
        for (size_t i=start_idx; i<end_idx; i++) {
            size_t row = inner_indices[i];
            Float val = values[i];
            insert(row, col) = val;
        }
    }
}
