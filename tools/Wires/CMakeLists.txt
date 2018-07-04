IF (CGAL_FOUND AND CORK_FOUND AND TETGEN_FOUND AND TRIANGLE_FOUND)
    FILE(GLOB SRC_FILES *.cpp)
    FILE(GLOB INC_FILES *.h)
    FILE(GLOB DEP_FILES *.i)

    ADD_SUBDIRECTORY(Attributes)
    ADD_SUBDIRECTORY(WireNetwork)
    ADD_SUBDIRECTORY(Tiler)
    ADD_SUBDIRECTORY(Misc)
    ADD_SUBDIRECTORY(Inflator)
    ADD_SUBDIRECTORY(Interfaces)
    ADD_SUBDIRECTORY(Parameters)

    ADD_LIBRARY(lib_Wires SHARED ${SRC_FILES} ${INC_FILES})
    TARGET_LINK_LIBRARIES(lib_Wires
        PUBLIC
            Mesh
            PyMesh::Tools
            lib_CGAL
        PRIVATE
            lib_MeshUtils
            lib_ConvexHull
            lib_Triangle
            lib_Boolean
            lib_TetGen
    )
    SET_TARGET_PROPERTIES(lib_Wires PROPERTIES OUTPUT_NAME "PyMesh-Wires")
ENDIF (CGAL_FOUND AND CORK_FOUND AND TETGEN_FOUND AND TRIANGLE_FOUND)
