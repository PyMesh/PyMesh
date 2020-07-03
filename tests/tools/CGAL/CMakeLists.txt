# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(CGAL_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(CGAL_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::CGAL
)
add_custom_target(run_CGAL_tests
    DEPENDS
        CGAL_tests
    COMMAND
        CGAL_tests
)
