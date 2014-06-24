#include "HashGrid.h"
#include "HashGridImplementation.h"
#include "HashMapTrait.h"

HashGrid::Ptr HashGrid::create(Float cell_size, HashGrid::ImplementationType impl_type) {
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
}
