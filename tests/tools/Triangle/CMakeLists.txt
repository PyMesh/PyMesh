# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Triangle_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Triangle_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Triangle
)
ADD_CUSTOM_TARGET(run_Triangle_tests
    COMMAND Triangle_tests
    DEPENDS Triangle_tests
)
