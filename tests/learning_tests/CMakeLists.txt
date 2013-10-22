# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(learning_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(learning_tests ${GTEST_BOTH_LIBRARIES})
