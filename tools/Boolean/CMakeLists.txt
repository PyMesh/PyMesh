# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(BooleanDependencies INTERFACE IMPORTED)

# Cork
IF (CORK_FOUND AND GMP_FOUND)
    ADD_SUBDIRECTORY(Cork)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            PyMesh::Gmp
            PyMesh::Cork
    )
    TARGET_COMPILE_DEFINITIONS(BooleanDependencies
        INTERFACE
            -DWITH_CORK
    )

ENDIF (CORK_FOUND AND GMP_FOUND)

# Clipper which requires triangle.
IF (CLIPPER_FOUND AND TRIANGLE_FOUND)
    ADD_SUBDIRECTORY(Clipper)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            PyMesh::Clipper
            lib_Triangle
    )
    TARGET_COMPILE_DEFINITIONS(BooleanDependencies
        INTERFACE
            -DWITH_CLIPPER
    )
ENDIF (CLIPPER_FOUND AND TRIANGLE_FOUND)

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    ADD_SUBDIRECTORY(IGL)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            lib_IGL
            lib_CGAL
    )
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

# CGAL
IF (CGAL_FOUND)
    ADD_SUBDIRECTORY(CGAL)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            lib_CGAL
    )
ENDIF (CGAL_FOUND)

# Carve
IF (CARVE_FOUND)
    ADD_SUBDIRECTORY(Carve)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            PyMesh::Carve
    )
    TARGET_COMPILE_DEFINITIONS(BooleanDependencies
        INTERFACE
            -DWITH_CARVE
    )
ENDIF (CARVE_FOUND)

# BSP
IF (BSP_FOUND)
    ADD_SUBDIRECTORY(BSP)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            PyMesh::BSP
    )
    TARGET_COMPILE_DEFINITIONS(BooleanDependencies
        INTERFACE
            -DWITH_BSP
    )
ENDIF (BSP_FOUND)

# tinyxml2
IF (TINYXML2_FOUND)
    TARGET_LINK_LIBRARIES(BooleanDependencies
        INTERFACE
            PyMesh::TinyXML2
    )
    TARGET_COMPILE_DEFINITIONS(BooleanDependencies
        INTERFACE
            -DWITH_TINYXML2
    )
ENDIF (TINYXML2_FOUND)

ADD_LIBRARY(lib_Boolean SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Boolean PROPERTIES OUTPUT_NAME "PyMesh-Boolean")
TARGET_LINK_LIBRARIES(lib_Boolean
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        BooleanDependencies
)
