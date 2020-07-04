# Enumerate source files
file(GLOB_RECURSE TEST_SRC_FILES *.cpp)
file(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

add_executable(Wires_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Wires_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Wires
        PyMesh::BVH
)
target_include_directories(Wires_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(run_Wires_tests
    DEPENDS
        Wires_tests
    COMMAND
        Wires_tests
)
