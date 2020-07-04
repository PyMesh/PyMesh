# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(Assembler_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Assembler_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Assembler
)
add_custom_target(run_Assembler_tests
    COMMAND
        Assembler_tests
    DEPENDS
        Assembler_tests
)
