# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools/SparseSolver/)

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *Test.h)

ADD_EXECUTABLE(SparseSolver_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(SparseSolver_tests Mesh lib_SparseSolver
    ${GTEST_BOTH_LIBRARIES})
ADD_CUSTOM_COMMAND(TARGET SparseSolver_tests POST_BUILD COMMAND SparseSolver_tests)
