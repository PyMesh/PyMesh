# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *Test.h)

add_executable(SparseSolver_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(SparseSolver_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::SparseSolver
)
add_custom_target(run_SparseSolver_tests
    DEPENDS
        SparseSolver_tests
    COMMAND
        SparseSolver_tests
)

