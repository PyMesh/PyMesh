# Find BSP  library (http://www.gilbertbernstein.com/project_boolean.html)
# The following variables are set
#
# BSP_FOUND
# BSP_INCLUDE_DIRS
# BSP_LIBRARIES
#
# It searches the environment variable $BSP_PATH automatically.

FIND_PATH(BSP_INCLUDE_DIRS bsp.hpp
    $ENV{BSP_PATH}
    $ENV{BSP_PATH}/include/
    $ENV{BSP_PATH}/source/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(BSP_LIBRARIES bsp
    $ENV{BSP_PATH}
    $ENV{BSP_PATH}/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BSP
    "BSP library cannot be found.  Consider set BSP_PATH environment variable"
    BSP_INCLUDE_DIRS
    BSP_LIBRARIES)

MARK_AS_ADVANCED(
    BSP_INCLUDE_DIRS
    BSP_LIBRARIES)

IF (BSP_FOUND AND NOT TARGET PyMesh::BSP)
    ADD_LIBRARY(PyMesh::BSP INTERFACE IMPORTED)
    TARGET_INCLUDE_DIRECTORIES(PyMesh::BSP SYSTEM
        INTERFACE
            ${BSP_INCLUDE_DIRS}
    )
    TARGET_LINK_LIBRARIES(PyMesh::BSP
        INTERFACE
            ${BSP_LIBRARIES}
    )
ENDIF (BSP_FOUND AND NOT TARGET PyMesh::BSP)
