# Source files
SET(SRC_FILES predicates.c)
SET(INC_FILES predicates.h)

ADD_LIBRARY(lib_Predicates STATIC ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Predicates PROPERTIES OUTPUT_NAME "PyMesh-Predicates")
TARGET_LINK_LIBRARIES(lib_Predicates PUBLIC PyMesh::Tools)

ADD_LIBRARY(PyMesh::Tools::Predicates ALIAS lib_Predicates)
