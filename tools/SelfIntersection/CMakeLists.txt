# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(IGL)
ADD_LIBRARY(lib_SelfIntersection SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_SelfIntersection PROPERTIES OUTPUT_NAME
    "PyMesh-SelfIntersection")
TARGET_LINK_LIBRARIES(lib_SelfIntersection
    PUBLIC
        PyMesh::Tools
        Mesh
    PRIVATE
        PyMesh::Tools::IGL
)

ADD_LIBRARY(PyMesh::Tools::SelfIntersection ALIAS lib_SelfIntersection)
