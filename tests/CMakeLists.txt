project(PyMeshTests)

# Add googletest googlemock support.
add_subdirectory(external/gmock-1.7.0)

# Create unit test interface target.
add_library(PyMesh::UnitTest INTERFACE IMPORTED)
target_link_libraries(PyMesh::UnitTest
    INTERFACE
        gtest
        gmock
        PyMesh::Mesh
)
target_include_directories(PyMesh::UnitTest SYSTEM
    INTERFACE
        ${gmock_SOURCE_DIR}
        ${gmock_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${gtest_SOURCE_DIR}/include
)
target_include_directories(PyMesh::UnitTest
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(PyMesh::UnitTest
    INTERFACE
        PYMESH_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

add_subdirectory(src)
add_subdirectory(tools)

add_custom_target(tests DEPENDS
    Mesh_tests
    run_Mesh_tests
    ToolsTests
)

