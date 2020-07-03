# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(Geogram_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Geogram_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Geogram
)
add_custom_target(run_Geogram_tests
    DEPENDS
        Geogram_tests
    COMMAND
        Geogram_tests
)
