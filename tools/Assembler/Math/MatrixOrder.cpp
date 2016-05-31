/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MatrixOrder.h"
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

MatrixI MatrixOrder::get_order(size_t dim) {
    MatrixI order(dim, dim);
    switch (dim) {
        case 2:
            order << 0, 2,
                     2, 1;
            break;
        case 3:
            order << 0, 5, 4,
                     5, 1, 3,
                     4, 3, 2;
            break;
        default:
            std::stringstream err_msg;
            err_msg << "Displacement to strain matrix is not implemented for "
                << dim << "D space.";
            throw NotImplementedError(err_msg.str());
    }
    return order;
}
