#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

namespace EigenSparseStack {
    ZSparseMatrix stack(
            const size_t num_row_blocks, const size_t num_column_blocks,
            std::vector<ZSparseMatrix*>& blocks);

}
