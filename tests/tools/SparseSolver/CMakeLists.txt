# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *Test.h)

ADD_EXECUTABLE(SparseSolver_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(SparseSolver_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::SparseSolver
)
ADD_CUSTOM_TARGET(run_SparseSolver_tests 
    DEPENDS
        SparseSolver_tests
    COMMAND
        SparseSolver_tests
)

