# Source files
set(SRC_FILES predicates.c)
set(INC_FILES predicates.h)

add_library(lib_Predicates STATIC ${SRC_FILES} ${INC_FILES})
set_target_properties(lib_Predicates PROPERTIES OUTPUT_NAME "PyMesh-Predicates")
target_link_libraries(lib_Predicates PUBLIC PyMesh::Tools)

add_library(PyMesh::Predicates ALIAS lib_Predicates)
