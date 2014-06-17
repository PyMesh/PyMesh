#include <gtest/gtest.h>
#include "BoundaryEdgesTest.h"
#include "BoundaryFacesTest.h"
#include "IndexHeapTest.h"
#include "IsolatedVertexRemovalTest.h"
#include "ShortEdgeRemovalTest.h"
#include "TripletMapTest.h"
#include "AttributeUtilsTest.h"
#include "PointLocatorTest.h"
#include "SimpleSubdivisionTest.h"
#include "DuplicatedVertexRemovalTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
