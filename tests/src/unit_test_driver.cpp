#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MeshTest.h"
#include "IO/OBJParserTest.h"
#include "IO/OBJWriterTest.h"
#include "Misc/TripletTest.h"
#include "Misc/HashGridTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
