#
# Find Jonathan Shewchuk's Triangle include directory and library, and set these variables:
#
# TRIANGLE_FOUND
# Triangle_INCLUDE_DIRS
# Triangle_LIBRARIES

# Optional user supplied search path.
set (Triangle_PREFIX_PATH "" CACHE PATH "Directory to search Triangle header and library files")

# Find include directory.
find_path (Triangle_INCLUDE_DIR triangle.h 
    ${Triangle_PREFIX_PATH}
    /opt/local/include
    /usr/local/include
    /usr/includes
)

# With Win32, important to have both
IF(WIN32)
  FIND_LIBRARY(Triangle_LIBRARIES triangle
               ${Triangle_PREFIX_PATH}
               /opt/local/lib
               /usr/local/lib
               /usr/lib)
ELSE(WIN32)
  # On unix system, debug and release have the same name
  FIND_LIBRARY(Triangle_LIBRARIES triangle
               ${Triangle_PREFIX_PATH}
               /opt/local/lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Triangle
  "Triangle could not be found."
  Triangle_LIBRARIES Triangle_INCLUDE_DIR)

mark_as_advanced(
  Triangle_INCLUDE_DIR
  Triangle_LIBRARIES
  )
