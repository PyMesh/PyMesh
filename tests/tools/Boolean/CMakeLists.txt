# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(Boolean_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Boolean_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Boolean
)
add_custom_target(run_Boolean_tests
    DEPENDS
        Boolean_tests
    COMMAND
        Boolean_tests
)

