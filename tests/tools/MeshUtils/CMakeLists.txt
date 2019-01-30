# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(MeshUtils_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(MeshUtils_tests 
    PRIVATE
        Mesh
        PyMesh::Tools::MeshUtils
        PyMesh::UnitTest)
ADD_CUSTOM_TARGET(run_MeshUtils_tests
    COMMAND
        MeshUtils_tests
    DEPENDS
        MeshUtils_tests
)
