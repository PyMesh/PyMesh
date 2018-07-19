IF (TRIANGLE_FOUND AND PYMESH_USE_TRIANGLE)
    SET(SRC_FILES TriangleWrapper.cpp)
    SET(INC_FILES TriangleWrapper.h)

    ADD_LIBRARY(lib_Triangle SHARED ${SRC_FILES} ${INC_FILES})
    SET_TARGET_PROPERTIES(lib_Triangle PROPERTIES OUTPUT_NAME "PyMesh-Triangle")
    TARGET_LINK_LIBRARIES(lib_Triangle 
        PUBLIC
            Mesh
            PyMesh::Tools
        PRIVATE
            lib_MeshUtils
            PyMesh::Triangle
    )
    TARGET_COMPILE_DEFINITIONS(lib_Triangle
        INTERFACE
            -DWITH_TRIANGLE
    )
ENDIF (TRIANGLE_FOUND AND PYMESH_USE_TRIANGLE)
