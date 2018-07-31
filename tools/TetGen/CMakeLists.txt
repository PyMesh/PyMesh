# Enumerate source files
FILE(GLOB_RECURSE SRC_FILES *.cpp)
FILE(GLOB_RECURSE INC_FILES *.h)

ADD_LIBRARY(lib_TetGen SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_TetGen PROPERTIES OUTPUT_NAME "PyMesh-TetGen")
TARGET_LINK_LIBRARIES(lib_TetGen
    PUBLIC
        Mesh
        PyMesh::Tools
        PyMesh::TetGen
)

ADD_LIBRARY(PyMesh::Tools::TetGen ALIAS lib_TetGen)
