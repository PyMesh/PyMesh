FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(Assemblers)
ADD_SUBDIRECTORY(Elements)
ADD_SUBDIRECTORY(FESetting)
ADD_SUBDIRECTORY(Math)
ADD_SUBDIRECTORY(Materials)
ADD_SUBDIRECTORY(Mesh)
ADD_SUBDIRECTORY(ShapeFunctions)

ADD_LIBRARY(lib_Assembler SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_Assembler
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::MeshUtils
)
SET_TARGET_PROPERTIES(lib_Assembler PROPERTIES OUTPUT_NAME "PyMesh-Assembler")

ADD_LIBRARY(PyMesh::Tools::Assembler ALIAS lib_Assembler)
