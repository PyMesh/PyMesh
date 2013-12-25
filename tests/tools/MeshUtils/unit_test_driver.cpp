#include <gtest/gtest.h>
#include "BoundaryEdgesTest.h"
#include "BoundaryFacesTest.h"
#include "IndexHeapTest.h"
#include "ShortEdgeRemovalTest.h"
#include "TripletMapTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
