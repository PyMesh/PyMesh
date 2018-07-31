# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(TetGen_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(TetGen_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::TetGen
)
ADD_CUSTOM_TARGET(run_TetGen_tests
    DEPENDS
        TetGen_tests
    COMMAND
        TetGen_tests
)
