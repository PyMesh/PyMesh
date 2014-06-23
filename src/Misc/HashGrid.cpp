#include "HashGrid.h"
#include "HashGridImplementation.h"
#include "HashMapTrait.h"

HashGrid::Ptr HashGrid::create(Float cell_size) {
    return Ptr(new HashGridImplementation<HashMapTrait<3, 2> >(cell_size));
}
