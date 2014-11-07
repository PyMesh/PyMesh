#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Attributes/WireAttributeTest.h"
#include "Attributes/WireEdgePeriodicIndexAttributeTest.h"
#include "Attributes/WireVertexMinAngleAttributeTest.h"
#include "Attributes/WireVertexPeriodicIndexAttributeTest.h"
#include "Inflator/BoxCheckerTest.h"
#include "Inflator/PeriodicInflator2DTest.h"
#include "Inflator/PeriodicInflator3DTest.h"
#include "Inflator/SimpleInflatorTest.h"
#include "Inflator/WireProfileTest.h"
#include "Misc/BilinearInterpolationTest.h"
#include "Misc/TrilinearInterpolationTest.h"
#include "Tiler/AABBTilerTest.h"
#include "Tiler/MeshTilerTest.h"
#include "WireNetwork/WireConnectivityTest.h"
#include "WireNetwork/WireNetworkTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
