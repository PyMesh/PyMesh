# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(ConvexHull_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(ConvexHull_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::ConvexHull
)
target_include_directories(ConvexHull_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(run_ConvexHull_tests
    DEPENDS
        ConvexHull_tests
    COMMAND
        ConvexHull_tests
)

