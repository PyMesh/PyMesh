#include "MatrixOrder.h"
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

MatrixI MatrixOrder::get_order(size_t dim) {
    MatrixI order(dim, dim);
    switch (dim) {
        case 2:
            order << 0, 2,
                  2, 1;
            break;
        case 3:
            order << 0, 3, 4,
                  3, 1, 5,
                  4, 5, 2;
            break;
        default:
            std::stringstream err_msg;
            err_msg << "Displacement to strain matrix is not implemented for "
                << dim << "D space.";
            throw NotImplementedError(err_msg.str());
    }
    return order;
}
