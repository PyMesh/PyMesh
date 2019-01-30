# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Tetrahedralization_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Tetrahedralization_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Tetrahedralization
        PyMesh::Tools::CGAL
        PyMesh::Tools::TetGen
        PyMesh::Tools::Geogram
        PyMesh::Quartet
        PyMesh::MMG
        PyMesh::TetWild
)
ADD_CUSTOM_TARGET(run_Tetrahedralization_tests
    DEPENDS
        Tetrahedralization_tests
    COMMAND
        Tetrahedralization_tests
)
