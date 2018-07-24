# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(EigenUtils_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(EigenUtils_tests
    PRIVATE
        lib_EigenUtils
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_EigenUtils_tests
    DEPENDS
        EigenUtils_tests
    COMMAND
        EigenUtils_tests
)
