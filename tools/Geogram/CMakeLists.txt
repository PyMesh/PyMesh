# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_Geogram SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_Geogram
    PUBLIC
        Mesh
        PyMesh::Tools
        PyMesh::Geogram
)
SET_TARGET_PROPERTIES(lib_Geogram PROPERTIES OUTPUT_NAME "PyMesh-Geogram")

ADD_LIBRARY(PyMesh::Tools::Geogram ALIAS lib_Geogram)
