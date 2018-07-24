/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#if WITH_DRACO
#include "Draco/DracoCompressionEngineTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
