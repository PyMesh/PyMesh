# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(IGL)
ADD_SUBDIRECTORY(FastWindingNumber)

ADD_LIBRARY(lib_WindingNumber SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_WindingNumber PROPERTIES OUTPUT_NAME
    "PyMesh-WindingNumber")
TARGET_LINK_LIBRARIES(lib_WindingNumber
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::IGL
        PyMesh::FastWindingNumber
)

ADD_LIBRARY(PyMesh::Tools::WindingNumber ALIAS lib_WindingNumber)
