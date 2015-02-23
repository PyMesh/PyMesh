#include <gtest/gtest.h>
#include "AttributeUtilsTest.h"
#include "BoundaryEdgesTest.h"
#include "BoundaryFacesTest.h"
#include "DuplicatedVertexRemovalTest.h"
#include "EdgeSplitterTest.h"
#include "FinFaceRemovalTest.h"
#include "IndexHeapTest.h"
#include "IsolatedVertexRemovalTest.h"
#include "LoopSubdivisionTest.h"
#include "LongEdgeRemovalTest.h"
#include "ObtuseTriangleRemovalTest.h"
#include "PointLocatorTest.h"
#include "ShortEdgeRemovalTest.h"
#include "SimpleSubdivisionTest.h"
#include "TripletMapTest.h"
#include "SubMeshTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
