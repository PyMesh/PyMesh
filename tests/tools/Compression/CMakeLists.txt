# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(Compression_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Compression_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Compression
)
add_custom_target(run_Compression_tests
    DEPENDS
        Compression_tests
    COMMAND
        Compression_tests
)

