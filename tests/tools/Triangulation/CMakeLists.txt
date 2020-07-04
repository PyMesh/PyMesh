# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(Triangulation_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Triangulation_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Triangulation
)
add_custom_target(run_Triangulation_tests
    DEPENDS
        Triangulation_tests
    COMMAND
        Triangulation_tests
)


