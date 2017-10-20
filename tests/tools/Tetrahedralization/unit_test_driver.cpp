/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include <gtest/gtest.h>
#ifdef WITH_TETGEN
#include "TetGen/TetGenEngineTest.h"
#endif

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
