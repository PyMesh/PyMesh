# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(ConvexHull_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(ConvexHull_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::ConvexHull
        PyMesh::Qhull
        PyMesh::Tools::CGAL
        PyMesh::Tools::Triangle
        PyMesh::Tools::TetGen
)
TARGET_INCLUDE_DIRECTORIES(ConvexHull_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
ADD_CUSTOM_TARGET(run_ConvexHull_tests
    DEPENDS
        ConvexHull_tests
    COMMAND
        ConvexHull_tests
)

