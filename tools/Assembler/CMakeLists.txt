file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.h)

add_subdirectory(Assemblers)
add_subdirectory(Elements)
add_subdirectory(FESetting)
add_subdirectory(Math)
add_subdirectory(Materials)
add_subdirectory(Mesh)
add_subdirectory(ShapeFunctions)

add_library(lib_Assembler SHARED ${SRC_FILES} ${INC_FILES})
target_link_libraries(lib_Assembler
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::MeshUtils
)
set_target_properties(lib_Assembler PROPERTIES OUTPUT_NAME "PyMesh-Assembler")

add_library(PyMesh::Assembler ALIAS lib_Assembler)
