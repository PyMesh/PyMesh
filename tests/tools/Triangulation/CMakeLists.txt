# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Triangulation_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Triangulation_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::MeshUtils
        PyMesh::Tools::Triangulation
        PyMesh::Tools::IGL
        PyMesh::Tools::CGAL
        PyMesh::Tools::Triangle
        PyMesh::Tools::Geogram
)
ADD_CUSTOM_TARGET(run_Triangulation_tests 
    DEPENDS
        Triangulation_tests
    COMMAND
        Triangulation_tests
)


