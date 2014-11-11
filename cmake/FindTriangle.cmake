# Find Jonathan Shewchuk's Triangle
# (http://www.cs.cmu.edu/~quake/triangle.html)
# The following variables are set.
#
# TRIANGLE_FOUND
# Triangle_INCLUDE_DIRS
# Triangle_LIBRARIES

# Find include directory.
find_path (Triangle_INCLUDE_DIRS triangle.h 
    $ENV{TRIANGLE_PATH}
    $ENV{TRIANGLE_PATH}/include
    /opt/local/include
    /usr/local/include
    /usr/includes
)

FIND_LIBRARY(Triangle_LIBRARIES triangle
           $ENV{TRIANGLE_PATH}
           $ENV{TRIANGLE_PATH}/lib
           /opt/local/lib
           /usr/local/lib
           /usr/lib)

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Triangle
  "Triangle could not be found."
  Triangle_LIBRARIES Triangle_INCLUDE_DIRS)

mark_as_advanced(
  Triangle_INCLUDE_DIRS
  Triangle_LIBRARIES
  )
