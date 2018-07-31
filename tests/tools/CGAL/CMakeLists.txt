# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(CGAL_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(CGAL_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::CGAL
)
ADD_CUSTOM_TARGET(run_CGAL_tests
    DEPENDS
        CGAL_tests
    COMMAND
        CGAL_tests
)
