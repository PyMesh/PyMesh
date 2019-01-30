# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(Draco)

ADD_LIBRARY(lib_Compression SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Compression PROPERTIES OUTPUT_NAME
    "PyMesh-Compression")
TARGET_LINK_LIBRARIES(lib_Compression
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Draco
)

ADD_LIBRARY(PyMesh::Tools::Compression ALIAS lib_Compression)
