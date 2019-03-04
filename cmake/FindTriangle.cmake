# Find Jonathan Shewchuk's Triangle
# (http://www.cs.cmu.edu/~quake/triangle.html)
# The following variables are set.
#
# TRIANGLE_FOUND
# TRIANGLE_INCLUDE_DIRS
# TRIANGLE_LIBRARIES

# Find include directory.
find_path (TRIANGLE_INCLUDE_DIRS triangle.h
    PATHS
    $ENV{TRIANGLE_PATH}
    $ENV{TRIANGLE_PATH}/include
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    NO_DEFAULT_PATH)

FIND_LIBRARY(TRIANGLE_LIBRARIES NAMES libtriangle.a triangle
    PATHS
    $ENV{TRIANGLE_PATH}
    $ENV{TRIANGLE_PATH}/lib
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    NO_DEFAULT_PATH)

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Triangle
  "Triangle could not be found."
  TRIANGLE_LIBRARIES TRIANGLE_INCLUDE_DIRS)

mark_as_advanced(
  TRIANGLE_INCLUDE_DIRS
  TRIANGLE_LIBRARIES
)

