# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(learning_unit_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(learning_unit_tests Mesh ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET learning_unit_tests POST_BUILD COMMAND learning_unit_tests)
SET(learning_test_dep learning_unit_tests)

# Find OpenCL
FIND_PACKAGE(OpenCL)
IF (OPENCL_FOUND)
    INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIRS})
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

    SET(OPENCL_SRC check_opencl.c cl-helper.c cl-helper.h)

    ADD_EXECUTABLE(check_opencl ${OPENCL_SRC})
    TARGET_LINK_LIBRARIES(check_opencl ${OPENCL_LIBRARIES})
    SET(learning_test_dep ${learning_test_dep} check_opencl)
ENDIF()

ADD_CUSTOM_TARGET(learning_tests DEPENDS ${learning_test_dep})
