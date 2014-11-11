# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *Test.h *.inl)

# Google sparse/dense hash
IF (SPARSEHASH_FOUND)
    ADD_DEFINITIONS(-DUSE_SPARSEHASH)
    INCLUDE_DIRECTORIES(${SPARSEHASH_INCLUDE_DIR})
ENDIF (SPARSEHASH_FOUND)

ADD_EXECUTABLE(src_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(src_tests ${GTEST_BOTH_LIBRARIES} Mesh)
ADD_CUSTOM_COMMAND(TARGET src_tests POST_BUILD COMMAND src_tests)

ADD_SUBDIRECTORY(Others)

# Bundle all target under the name test.
# To build tests, just "make tests"
ADD_CUSTOM_TARGET(tests DEPENDS src_tests other_tests)

