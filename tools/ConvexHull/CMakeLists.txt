# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")

# Inlclude CGAL
IF(CGAL_FOUND)
    ADD_DEFINITIONS(-DWITH_CGAL)
    LIST(APPEND DEPENDENT_LIBS cgal_wrapper)
    ADD_SUBDIRECTORY(CGAL)
ELSE(CGAL_FOUND)
    MESSAGE(WARNING "CGAL missing.  Skipping CGAL convex hull support")
ENDIF(CGAL_FOUND)

# Include Qhull
IF (QHULL_FOUND)
    INCLUDE_DIRECTORIES(${QHULL_INCLUDE_DIRS}/libqhull/)
    ADD_DEFINITIONS(-DWITH_QHULL)
    LIST(APPEND DEPENDENT_LIBS ${QHULL_LIBRARIES})
    ADD_SUBDIRECTORY(Qhull)
ELSE (QHULL_FOUND)
    MESSAGE(WARNING "Qhull missing.  Skipping qhull convex hull support")
ENDIF (QHULL_FOUND)

#Include Triangle
IF (TRIANGLE_FOUND)
    ADD_DEFINITIONS(-DWITH_TRIANGLE)
    LIST(APPEND DEPENDENT_LIBS lib_triangle_wrapper)
    ADD_SUBDIRECTORY(Triangle)
ELSE (TRIANGLE_FOUND)
    MESSAGE(WARNING "Triangle is missing.  Skipping triangle convex hull support")
ENDIF (TRIANGLE_FOUND)

#Include Tetgen
IF (TETGEN_FOUND)
    ADD_DEFINITIONS(-DWITH_TETGEN)
    LIST(APPEND DEPENDENT_LIBS lib_tetgen_wrapper)
    ADD_SUBDIRECTORY(TetGen)
ELSE (TETGEN_FOUND)
    MESSAGE(WARNING "Tetgen is missing.  Skipping tetgen convex hull support")
ENDIF (TETGEN_FOUND)

ADD_LIBRARY(lib_convex_hull SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_convex_hull
    ${DEPENDENT_LIBS} Mesh lib_MeshUtils)
SET_TARGET_PROPERTIES(lib_convex_hull PROPERTIES OUTPUT_NAME "convex_hull")

ADD_CUSTOM_TARGET(convex_hull DEPENDS lib_convex_hull)

