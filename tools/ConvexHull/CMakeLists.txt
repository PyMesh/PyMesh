# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(ConvexHullDependencies INTERFACE IMPORTED)

# Inlclude CGAL
IF(CGAL_FOUND AND PYMESH_USE_CGAL)
    ADD_SUBDIRECTORY(CGAL)
    TARGET_LINK_LIBRARIES(ConvexHullDependencies 
        INTERFACE
            lib_CGAL
    )
ENDIF(CGAL_FOUND AND PYMESH_USE_CGAL)

# Include Qhull
IF (QHULL_FOUND AND PYMESH_USE_QHULL)
    ADD_SUBDIRECTORY(Qhull)
    TARGET_LINK_LIBRARIES(ConvexHullDependencies 
        INTERFACE
            QHULL::Qhull
    )
    TARGET_COMPILE_DEFINITIONS(ConvexHullDependencies
        INTERFACE
            -DWITH_QHULL
    )
ENDIF (QHULL_FOUND AND PYMESH_USE_QHULL)

#Include Triangle
IF (TRIANGLE_FOUND AND PYMESH_USE_TRIANGLE)
    ADD_SUBDIRECTORY(Triangle)
    TARGET_LINK_LIBRARIES(ConvexHullDependencies 
        INTERFACE
            lib_Triangle
    )
ENDIF (TRIANGLE_FOUND AND PYMESH_USE_TRIANGLE)

#Include Tetgen
IF (TETGEN_FOUND AND PYMESH_USE_TETGEN)
    ADD_SUBDIRECTORY(TetGen)
    TARGET_LINK_LIBRARIES(ConvexHullDependencies 
        INTERFACE
            lib_TetGen
    )
ENDIF (TETGEN_FOUND AND PYMESH_USE_TETGEN)

ADD_LIBRARY(lib_ConvexHull SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_ConvexHull
    PUBLIC
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        ConvexHullDependencies
)
SET_TARGET_PROPERTIES(lib_ConvexHull PROPERTIES OUTPUT_NAME
    "PyMesh-ConvexHull")

