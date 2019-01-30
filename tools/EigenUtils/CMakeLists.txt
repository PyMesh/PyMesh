# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_EigenUtils SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_EigenUtils PROPERTIES OUTPUT_NAME "PyMesh-EigenUtiles")
TARGET_LINK_LIBRARIES(lib_EigenUtils 
    PUBLIC
        Mesh
        PyMesh::Tools
)
