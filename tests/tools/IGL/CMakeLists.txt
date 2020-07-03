# Enumerate source files
file(GLOB TEST_SRC_FILES unit_test_driver.cpp)
file(GLOB TEST_INC_FILES *_test.h)

add_executable(IGL_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
target_link_libraries(IGL_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::IGL
)
add_custom_target(run_IGL_tests
    DEPENDS
        IGL_tests
    COMMAND
        IGL_tests
)
