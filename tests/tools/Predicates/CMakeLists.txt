# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(Predicates_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Predicates_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Predicates
)
add_custom_target(run_Predicates_tests
    DEPENDS
        Predicates_tests
    COMMAND
        Predicates_tests
)
