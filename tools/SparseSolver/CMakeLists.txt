# Enumerate source files
FILE(GLOB_RECURSE SRC_FILES *.cpp)
FILE(GLOB_RECURSE INC_FILES *.h *.hh)

ADD_LIBRARY(lib_SparseSolver SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_SparseSolver PROPERTIES OUTPUT_NAME
    "PyMesh-SparseSolver")
TARGET_LINK_LIBRARIES(lib_SparseSolver
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Umfpack
        PyMesh::Cholmod
        PyMesh::Metis
        PyMesh::MKL
)

ADD_LIBRARY(PyMesh::Tools::SparseSolver ALIAS lib_SparseSolver)
