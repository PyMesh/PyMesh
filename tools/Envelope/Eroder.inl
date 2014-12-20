#include <cassert>
#include <vector>

template<int DIM>
void Eroder<DIM>::erode_2D() {
    Vector_i grid_size = m_mask.size();
    assert(grid_size.size() == 2);

    Mask result = m_mask;

    const size_t x_cells = grid_size[0];
    const size_t y_cells = grid_size[1];
    for (size_t i=0; i<x_cells; i++) {
        for (size_t j=0; j<y_cells; j++) {
            Vector_i curr_idx(i, j);
            if (!m_mask(curr_idx)) continue;

            std::vector<Vector_i> neighbors = {
                Vector_i(curr_idx[0]-1, curr_idx[1]-1),
                Vector_i(curr_idx[0]  , curr_idx[1]-1),
                Vector_i(curr_idx[0]+1, curr_idx[1]-1),
                Vector_i(curr_idx[0]-1, curr_idx[1]  ),
                Vector_i(curr_idx[0]  , curr_idx[1]  ),
                Vector_i(curr_idx[0]+1, curr_idx[1]  ),
                Vector_i(curr_idx[0]+1, curr_idx[1]+1),
                Vector_i(curr_idx[0]  , curr_idx[1]+1),
                Vector_i(curr_idx[0]-1, curr_idx[1]+1)
            };

            bool should_be_filled = true;
            for (const auto& neighbor : neighbors) {
                if (m_mask.is_valid_index(neighbor)) {
                    should_be_filled = should_be_filled && m_mask(neighbor);
                }
            }
            result(curr_idx) = should_be_filled;
        }
    }

    m_mask = result;
}

template<int DIM>
void Eroder<DIM>::erode_3D() {
    Vector_i grid_size = m_mask.size();
    assert(grid_size.size() == 3);

    Mask result = m_mask;

    const size_t x_cells = grid_size[0];
    const size_t y_cells = grid_size[1];
    const size_t z_cells = grid_size[2];
    for (size_t i=0; i<x_cells; i++) {
        for (size_t j=0; j<y_cells; j++) {
            for (size_t k=0; k<z_cells; k++) {
                Vector_i curr_idx(i, j, k);
                if (!m_mask(curr_idx)) continue;

                std::vector<Vector_i> neighbors = {
                    Vector_i(curr_idx[0]-1, curr_idx[1]-1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]-1, curr_idx[1]-1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]-1, curr_idx[1]-1, curr_idx[2]+1),
                    Vector_i(curr_idx[0]-1, curr_idx[1]  , curr_idx[2]-1),
                    Vector_i(curr_idx[0]-1, curr_idx[1]  , curr_idx[2]  ),
                    Vector_i(curr_idx[0]-1, curr_idx[1]  , curr_idx[2]+1),
                    Vector_i(curr_idx[0]-1, curr_idx[1]+1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]-1, curr_idx[1]+1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]-1, curr_idx[1]+1, curr_idx[2]+1),
                    Vector_i(curr_idx[0]  , curr_idx[1]-1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]  , curr_idx[1]-1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]  , curr_idx[1]-1, curr_idx[2]+1),
                    Vector_i(curr_idx[0]  , curr_idx[1]  , curr_idx[2]-1),
                    Vector_i(curr_idx[0]  , curr_idx[1]  , curr_idx[2]  ),
                    Vector_i(curr_idx[0]  , curr_idx[1]  , curr_idx[2]+1),
                    Vector_i(curr_idx[0]  , curr_idx[1]+1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]  , curr_idx[1]+1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]  , curr_idx[1]+1, curr_idx[2]+1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]-1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]-1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]+1, curr_idx[1]-1, curr_idx[2]+1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]  , curr_idx[2]-1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]  , curr_idx[2]  ),
                    Vector_i(curr_idx[0]+1, curr_idx[1]  , curr_idx[2]+1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]+1, curr_idx[2]-1),
                    Vector_i(curr_idx[0]+1, curr_idx[1]+1, curr_idx[2]  ),
                    Vector_i(curr_idx[0]+1, curr_idx[1]+1, curr_idx[2]+1)
                };

                bool should_be_filled = true;
                for (const auto& neighbor : neighbors) {
                    if (m_mask.is_valid_index(neighbor)) {
                        should_be_filled = should_be_filled && m_mask(neighbor);
                    }
                }
                result(curr_idx) = should_be_filled;
            }
        }
    }

    m_mask = result;
}
