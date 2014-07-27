#include "eigen_sparse_stack.h"

#include <cassert>

using namespace EigenSparseStack;

ZSparseMatrix EigenSparseStack::stack(
        const size_t num_row_blocks, const size_t num_column_blocks,
        std::vector<ZSparseMatrix*>& blocks) {
    assert(num_row_blocks > 0 && num_column_blocks > 0);
    assert(num_row_blocks * num_column_blocks == blocks.size());

    size_t num_rows = 0, num_columns = 0;
    for (size_t i=0; i<num_row_blocks; i++) {
        num_rows += blocks[i*num_column_blocks]->rows();
    }
    for (size_t i=0; i<num_column_blocks; i++) {
        num_columns += blocks[i]->cols();
    }

    ZSparseMatrix result(num_rows, num_columns);
    size_t row_count = 0, col_count = 0;
    for (size_t i=0; i<num_row_blocks; i++) {
        col_count = 0;
        size_t rows = 0;
        for (size_t j=0; j<num_column_blocks; j++) {
            size_t index = i*num_column_blocks + j;
            const ZSparseMatrix* block = blocks[index];
            result.block(row_count, col_count,
                    block->rows(), block->cols()) = *block;
            col_count += block->cols();
            rows = block->rows();
        }
        row_count += rows;
    }

    return result;
}
