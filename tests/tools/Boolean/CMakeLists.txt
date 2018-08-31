# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Boolean_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Boolean_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Boolean
        PyMesh::Cork
        PyMesh::Clipper
        PyMesh::Carve
        PyMesh::BSP
        PyMesh::TinyXML2
        PyMesh::Tools::Triangle
        PyMesh::Tools::IGL
        PyMesh::Tools::CGAL
)
ADD_CUSTOM_TARGET(run_Boolean_tests
    DEPENDS
        Boolean_tests
    COMMAND
        Boolean_tests
)

