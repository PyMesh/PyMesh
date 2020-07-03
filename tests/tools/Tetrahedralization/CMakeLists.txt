# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(Tetrahedralization_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(Tetrahedralization_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tetrahedralization
)
add_custom_target(run_Tetrahedralization_tests
    DEPENDS
        Tetrahedralization_tests
    COMMAND
        Tetrahedralization_tests
)
