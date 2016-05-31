#include <cassert>
#include <vector>

using namespace PyMesh;

namespace EroderHelper {
    void erode(Eroder<2>::Mask& mask) {
        typedef Eroder<2>::Vector_i Vector_i;
        Vector_i grid_size = mask.size();
        assert(grid_size.size() == 2);

        Eroder<2>::Mask result = mask;

        const size_t x_cells = grid_size[0];
        const size_t y_cells = grid_size[1];
        for (size_t i=0; i<x_cells; i++) {
            for (size_t j=0; j<y_cells; j++) {
                Vector_i curr_idx(i, j);
                if (!mask(curr_idx)) continue;

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
                    if (mask.is_valid_index(neighbor)) {
                        should_be_filled = should_be_filled && mask(neighbor);
                    }
                }
                result(curr_idx) = should_be_filled;
            }
        }

        mask = result;
    }

    void erode(Eroder<3>::Mask& mask) {
        typedef Eroder<3>::Vector_i Vector_i;
        Vector_i grid_size = mask.size();
        assert(grid_size.size() == 3);

        Eroder<3>::Mask result = mask;

        const size_t x_cells = grid_size[0];
        const size_t y_cells = grid_size[1];
        const size_t z_cells = grid_size[2];
        for (size_t i=0; i<x_cells; i++) {
            for (size_t j=0; j<y_cells; j++) {
                for (size_t k=0; k<z_cells; k++) {
                    Vector_i curr_idx(i, j, k);
                    if (!mask(curr_idx)) continue;

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
                        if (mask.is_valid_index(neighbor)) {
                            should_be_filled = should_be_filled && mask(neighbor);
                        }
                    }
                    result(curr_idx) = should_be_filled;
                }
            }
        }

        mask = result;
    }
}
using namespace EroderHelper;

template<int DIM>
void Eroder<DIM>::erode(size_t iterations) {
    for (size_t i=0; i<iterations; i++)
        EroderHelper::erode(m_mask);
}
