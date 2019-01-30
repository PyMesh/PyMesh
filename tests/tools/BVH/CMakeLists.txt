# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(BVH_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(BVH_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::BVH
        PyMesh::Tools::CGAL
        PyMesh::Tools::Geogram
        PyMesh::Tools::IGL
)
ADD_CUSTOM_TARGET(run_BVH_tests
    DEPENDS
        BVH_tests
    COMMAND
        BVH_tests
)
