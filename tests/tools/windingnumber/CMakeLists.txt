# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(windingnumber_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(windingnumber_tests Mesh lib_windingnumber ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET windingnumber_tests POST_BUILD COMMAND windingnumber_tests)

