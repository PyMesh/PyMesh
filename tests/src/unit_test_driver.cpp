#include <gtest/gtest.h>
#include "MeshTest.h"
#include "TripletTest.h"
#include "HashGridTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
