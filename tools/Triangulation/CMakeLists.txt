# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(TriangulationDependencies INTERFACE IMPORTED)

# IGL and CGAL
IF (TARGET lib_IGL AND TARGET lib_CGAL AND PYMESH_USE_LIBIGL AND PYMESH_USE_CGAL)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_IGL
            lib_CGAL
    )
    ADD_SUBDIRECTORY(IGL)
    ADD_SUBDIRECTORY(CGAL)
ENDIF (TARGET lib_IGL AND TARGET lib_CGAL AND PYMESH_USE_LIBIGL AND PYMESH_USE_CGAL)

# Shewchuk's Triangle
IF (TARGET lib_Triangle AND PYMESH_USE_TRIANGLE)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_Triangle
    )
    ADD_SUBDIRECTORY(ShewchukTriangle)
ENDIF (TARGET lib_Triangle AND PYMESH_USE_TRIANGLE)

# Geogram
IF (TARGET lib_Geogram AND PYMESH_USE_GEOGRAM)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_Geogram
    )
    ADD_SUBDIRECTORY(Geogram)
ENDIF (TARGET lib_Geogram AND PYMESH_USE_GEOGRAM)

ADD_LIBRARY(lib_Triangulation SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Triangulation PROPERTIES OUTPUT_NAME
    "PyMesh-Triangulation")
TARGET_LINK_LIBRARIES(lib_Triangulation
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        TriangulationDependencies
)
