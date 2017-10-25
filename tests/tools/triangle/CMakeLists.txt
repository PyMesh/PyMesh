# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(triangle_tests_bin ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(triangle_tests_bin Mesh lib_triangle_wrapper ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_TARGET(triangle_tests COMMAND triangle_tests_bin DEPENDS triangle_tests_bin)
SET_TARGET_PROPERTIES(triangle_tests_bin PROPERTIES OUTPUT_NAME triangle_tests)
#ADD_CUSTOM_COMMAND(TARGET triangle_tests POST_BUILD COMMAND triangle_tests)
