# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(Triangle_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Triangle_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Triangle
)
add_custom_target(run_Triangle_tests
    COMMAND Triangle_tests
    DEPENDS Triangle_tests
)
