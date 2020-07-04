# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(BVH_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(BVH_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::BVH
)
add_custom_target(run_BVH_tests
    DEPENDS
        BVH_tests
    COMMAND
        BVH_tests
)
