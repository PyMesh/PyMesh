# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_MeshUtils SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_MeshUtils PROPERTIES OUTPUT_NAME "PyMesh-MeshUtils")
TARGET_LINK_LIBRARIES(lib_MeshUtils
    PUBLIC
        Mesh
        lib_Predicates
        PyMesh::Tools
)

ADD_CUSTOM_TARGET(MeshUtils DEPENDS lib_MeshUtils)
