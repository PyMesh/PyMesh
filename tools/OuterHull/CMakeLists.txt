# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(OuterHullDependencies INTERFACE IMPORTED)

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    ADD_SUBDIRECTORY(IGL)
    TARGET_LINK_LIBRARIES(OuterHullDependencies
        INTERFACE
            lib_CGAL
            lib_IGL
    )
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

ADD_LIBRARY(lib_OuterHull SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_OuterHull PROPERTIES OUTPUT_NAME "PyMesh-OuterHull")
TARGET_LINK_LIBRARIES(lib_OuterHull
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        lib_SelfIntersection
        OuterHullDependencies
)

