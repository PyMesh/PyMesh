# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(TriangulationDependencies INTERFACE IMPORTED)

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_IGL
            lib_CGAL
    )
    ADD_SUBDIRECTORY(IGL)
    ADD_SUBDIRECTORY(CGAL)
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

# Shewchuk's Triangle
IF (TRIANGLE_FOUND)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_Triangle
    )
    ADD_SUBDIRECTORY(ShewchukTriangle)
ENDIF (TRIANGLE_FOUND)

# Geogram
IF (GEOGRAM_FOUND)
    TARGET_LINK_LIBRARIES(TriangulationDependencies
        INTERFACE
            lib_Geogram
    )
    ADD_SUBDIRECTORY(Geogram)
ENDIF (GEOGRAM_FOUND)

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
