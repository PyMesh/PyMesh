#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MeshTest.h"
#include "TripletTest.h"
#include "HashGridTest.h"
#include "OBJParserTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
