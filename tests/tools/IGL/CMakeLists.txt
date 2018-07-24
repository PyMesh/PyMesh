# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(IGL_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(IGL_tests
    PRIVATE
        lib_IGL
        lib_MeshUtils
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_IGL_tests
    DEPENDS
        IGL_tests
    COMMAND
        IGL_tests
)
