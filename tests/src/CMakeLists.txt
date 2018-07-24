# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *Test.h *.inl)

ADD_EXECUTABLE(Mesh_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Mesh_tests
    PRIVATE
        Mesh
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_Mesh_tests
    COMMAND
        Mesh_tests
    DEPENDS
        Mesh_tests
)
