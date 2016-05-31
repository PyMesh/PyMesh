/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <sstream>

#include "HashGrid.h"
#include "HashGridImplementation.h"
#include "HashMapTrait.h"

#include <Core/Exception.h>

using namespace PyMesh;

HashGrid::Ptr HashGrid::create(Float cell_size, size_t dim, HashGrid::ImplementationType impl_type) {
    if (dim == 3) {
        switch (impl_type) {
            case SPARSE_HASH:
                return Ptr(new HashGridImplementation<HashMapTrait<3, 1> >(cell_size));
                break;
            case DENSE_HASH:
                return Ptr(new HashGridImplementation<HashMapTrait<3, 2> >(cell_size));
                break;
            default:
                return Ptr(new HashGridImplementation<HashMapTrait<3, 0> >(cell_size));
        }
    } else if (dim == 2) {
        switch (impl_type) {
            case SPARSE_HASH:
                return Ptr(new HashGridImplementation<HashMapTrait<2, 1> >(cell_size));
                break;
            case DENSE_HASH:
                return Ptr(new HashGridImplementation<HashMapTrait<2, 2> >(cell_size));
                break;
            default:
                return Ptr(new HashGridImplementation<HashMapTrait<2, 0> >(cell_size));
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Hash grid does not support " << dim << "D space";
        throw NotImplementedError(err_msg.str());
    }
}

