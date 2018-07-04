# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(CompressionDependencies INTERFACE IMPORTED)

# Draco
IF (draco_FOUND)
    TARGET_LINK_LIBRARIES(CompressionDependencies
        INTERFACE
            PyMesh::Draco
    )
    TARGET_COMPILE_DEFINITIONS(CompressionDependencies
        INTERFACE
            -DWITH_DRACO
    )
    ADD_SUBDIRECTORY(Draco)
ENDIF (draco_FOUND)

ADD_LIBRARY(lib_Compression SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Compression PROPERTIES OUTPUT_NAME
    "PyMesh-Compression")
TARGET_LINK_LIBRARIES(lib_Compression
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        CompressionDependencies
)
