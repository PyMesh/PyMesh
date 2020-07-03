# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB_RECURSE TEST_INC_FILES *Test.h *.inl)

add_executable(Mesh_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Mesh_tests
    PRIVATE
        PyMesh::Mesh
        PyMesh::UnitTest
)
add_custom_target(run_Mesh_tests
    COMMAND
        Mesh_tests
    DEPENDS
        Mesh_tests
)
