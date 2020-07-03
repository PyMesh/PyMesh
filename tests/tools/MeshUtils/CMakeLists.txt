# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(MeshUtils_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(MeshUtils_tests
    PRIVATE
        Mesh
        PyMesh::MeshUtils
        PyMesh::UnitTest)
add_custom_target(run_MeshUtils_tests
    COMMAND
        MeshUtils_tests
    DEPENDS
        MeshUtils_tests
)
