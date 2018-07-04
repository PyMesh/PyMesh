IF (CGAL_FOUND AND LIBIGL_FOUND)
    # Enumerate source files
    FILE(GLOB SRC_FILES *.cpp *.inl)
    FILE(GLOB INC_FILES *.h)

    ADD_LIBRARY(lib_IGL SHARED ${SRC_FILES} ${INC_FILES})
    SET_TARGET_PROPERTIES(lib_IGL PROPERTIES OUTPUT_NAME "PyMesh-IGL")
    TARGET_LINK_LIBRARIES(lib_IGL
        PUBLIC
            lib_CGAL
            PyMesh::Tools
            PyMesh::libigl
    )
    TARGET_COMPILE_DEFINITIONS(lib_IGL
        INTERFACE
            -DWITH_IGL
    )
ENDIF (CGAL_FOUND AND LIBIGL_FOUND)
