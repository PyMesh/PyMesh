IF (GEOGRAM_FOUND AND PYMESH_USE_GEOGRAM)
    # Enumerate source files
    FILE(GLOB SRC_FILES *.cpp *.inl)
    FILE(GLOB INC_FILES *.h)

    ADD_LIBRARY(lib_Geogram SHARED ${SRC_FILES} ${INC_FILES})
    TARGET_LINK_LIBRARIES(lib_Geogram
        PUBLIC
            Mesh
            Geogram::geogram
            PyMesh::Tools
    )
    TARGET_COMPILE_DEFINITIONS(lib_Geogram
        PUBLIC
            -DWITH_GEOGRAM
    )
    SET_TARGET_PROPERTIES(lib_Geogram PROPERTIES OUTPUT_NAME "PyMesh-Geogram")
ENDIF (GEOGRAM_FOUND AND PYMESH_USE_GEOGRAM)
