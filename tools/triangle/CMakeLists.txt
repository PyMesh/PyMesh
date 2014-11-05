# General lib locations
LINK_DIRECTORIES(/opt/local/lib)

# Find triangle
IF (DEFINED ENV{TRIANGLE_PATH})
    SET(Triangle_PREFIX_PATH $ENV{TRIANGLE_PATH})
ENDIF (DEFINED ENV{TRIANGLE_PATH})
FIND_PACKAGE(Triangle REQUIRED)
INCLUDE_DIRECTORIES(${Triangle_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
SET(SRC_FILES TriangleWrapper.cpp)
SET(INC_FILES TriangleWrapper.h)

ADD_LIBRARY(lib_triangle_wrapper SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_triangle_wrapper PROPERTIES OUTPUT_NAME "triangle_wrapper")
TARGET_LINK_LIBRARIES(lib_triangle_wrapper Mesh lib_MeshUtils ${Triangle_LIBRARIES})

ADD_CUSTOM_TARGET(triangle DEPENDS lib_triangle_wrapper)
