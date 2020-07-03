# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(TetGen_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(TetGen_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::TetGen
)
add_custom_target(run_TetGen_tests
    DEPENDS
        TetGen_tests
    COMMAND
        TetGen_tests
)
