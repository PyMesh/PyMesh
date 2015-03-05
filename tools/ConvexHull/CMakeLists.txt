# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")

# Inlclude CGAL
IF(CGAL_FOUND)
    INCLUDE(${CGAL_USE_FILE})
    ADD_DEFINITIONS(-DWITH_CGAL)
    LIST(APPEND DEPENDENT_LIBS ${CGAL_LIBRARIES})
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


ADD_LIBRARY(lib_convex_hull SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_convex_hull
    ${DEPENDENT_LIBS} Mesh lib_MeshUtils)
SET_TARGET_PROPERTIES(lib_convex_hull PROPERTIES OUTPUT_NAME "convex_hull")

IF(NOT WITHOUT_SWIG)
    SET(SWG_FILES PyConvexHull.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyConvexHull python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyConvexHull ${PYTHON_LIBRARIES} lib_convex_hull)

    # Boudle exe, lib, swig wrapper into a single target.
    # To build all: "make convex_hull"
    ADD_CUSTOM_TARGET(convex_hull DEPENDS lib_convex_hull _PyConvexHull)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(convex_hull DEPENDS lib_convex_hull)
ENDIF(NOT WITHOUT_SWIG)

