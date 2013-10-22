# Include Eigen
IF (DEFINED ENV{EIGEN_INC})
    SET(EIGEN_BASE $ENV{EIGEN_INC}/eigen3)
ELSE (DEFINED ENV{EIGEN_INC})
    SET(EIGEN_BASE "/opt/local/include/eigen3")
ENDIF (DEFINED ENV{EIGEN_INC})
INCLUDE_DIRECTORIES("/opt/local/include")
INCLUDE_DIRECTORIES(${EIGEN_BASE})
INCLUDE_DIRECTORIES(${EIGEN_BASE}/Eigen)
INCLUDE_DIRECTORIES(${EIGEN_BASE}/unsupported)
ADD_DEFINITIONS(-DEIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET)

# Enumerate source files
SET(SRC_FILES example.cpp)

# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Add executable and library
ADD_EXECUTABLE(example EXCLUDE_FROM_ALL ${SRC_FILES})
TARGET_LINK_LIBRARIES(example mesh)

# Find googletest
FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ENABLE_TESTING()
ADD_EXECUTABLE(unit_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(unit_tests ${GTEST_BOTH_LIBRARIES})
ADD_TEST(all_tests unit_tests)

# Bundle all target under the name test.
# To build all tests, just "make tests"
ADD_CUSTOM_TARGET(tests DEPENDS example unit_tests)
