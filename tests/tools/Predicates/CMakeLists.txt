# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools/Predicates/)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(predicates_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(predicates_tests Mesh lib_predicates ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET predicates_tests POST_BUILD COMMAND predicates_tests)
