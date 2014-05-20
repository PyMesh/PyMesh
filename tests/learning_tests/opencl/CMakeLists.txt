# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

# Define kernel path
ADD_DEFINITIONS(-DKERNEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/opencl_kernels/")

ADD_EXECUTABLE(opencl_unit_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(opencl_unit_tests Mesh lib_opencl_wrapper ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET opencl_unit_tests POST_BUILD COMMAND opencl_unit_tests)
SET(opencl_test_dep opencl_unit_tests)

SET(OPENCL_SRC check_opencl.c cl-helper.c cl-helper.h)
ADD_EXECUTABLE(check_opencl ${OPENCL_SRC})
TARGET_LINK_LIBRARIES(check_opencl ${OPENCL_LIBRARIES})
SET(opencl_test_dep ${opencl_test_dep} check_opencl)

ADD_CUSTOM_TARGET(opencl_tests DEPENDS ${opencl_test_dep})
