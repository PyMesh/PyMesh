#include <gtest/gtest.h>
#include <gmock/gmock.h>
#ifdef WITH_CORK
#include "Cork/CorkEngineTest.h"
#endif
#ifdef WITH_CLIPPER
#include "Clipper/ClipperEngineTest.h"
#endif
#ifdef WITH_IGL
#include "IGL/IGLEngineTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
