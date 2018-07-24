PROJECT(PyMeshTests)

# Add googletest googlemock support.
ADD_SUBDIRECTORY(external/gmock-1.7.0)

# Create unit test interface target.
ADD_LIBRARY(PyMesh::UnitTest INTERFACE IMPORTED)
TARGET_LINK_LIBRARIES(PyMesh::UnitTest
    INTERFACE
        gtest
        gmock
        Mesh
)
TARGET_INCLUDE_DIRECTORIES(PyMesh::UnitTest SYSTEM
    INTERFACE
        ${gmock_SOURCE_DIR}
        ${gmock_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${gtest_SOURCE_DIR}/include
)
TARGET_INCLUDE_DIRECTORIES(PyMesh::UnitTest
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
TARGET_COMPILE_DEFINITIONS(PyMesh::UnitTest
    INTERFACE
        PYMESH_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(learning_tests)

ADD_CUSTOM_TARGET(tests DEPENDS 
    Mesh_tests
    run_Mesh_tests
    ToolsTests
)

