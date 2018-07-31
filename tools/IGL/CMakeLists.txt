# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_IGL SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_IGL PROPERTIES OUTPUT_NAME "PyMesh-IGL")
TARGET_LINK_LIBRARIES(lib_IGL
    PUBLIC
        PyMesh::Tools
        PyMesh::libigl
        PyMesh::Tools::CGAL
)

ADD_LIBRARY(PyMesh::Tools::IGL ALIAS lib_IGL)
