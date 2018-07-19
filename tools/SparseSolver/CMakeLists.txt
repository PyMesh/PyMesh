# Enumerate source files
FILE(GLOB_RECURSE SRC_FILES *.cpp)
FILE(GLOB_RECURSE INC_FILES *.h *.hh)

ADD_LIBRARY(SparseSolverDependencies INTERFACE IMPORTED)

IF (UMFPACK_FOUND AND PYMESH_USE_UMFPACK)
    TARGET_LINK_LIBRARIES(SparseSolverDependencies
        INTERFACE
            PyMesh::Umfpack
    )
    TARGET_COMPILE_DEFINITIONS(SparseSolverDependencies
        INTERFACE
            -DWITH_UMFPACK
    )
ENDIF (UMFPACK_FOUND AND PYMESH_USE_UMFPACK)

IF (CHOLMOD_FOUND AND PYMESH_USE_CHOLMOD)
    TARGET_LINK_LIBRARIES(SparseSolverDependencies
        INTERFACE
            PyMesh::Cholmod
    )
    TARGET_COMPILE_DEFINITIONS(SparseSolverDependencies
        INTERFACE
            -DWITH_CHOLMOD
    )
ENDIF (CHOLMOD_FOUND AND PYMESH_USE_CHOLMOD)

IF (METIS_FOUND AND PYMESH_USE_METIS)
    TARGET_LINK_LIBRARIES(SparseSolverDependencies
        INTERFACE
            PyMesh::Metis
    )
ENDIF (METIS_FOUND AND PYMESH_USE_METIS)

IF (MKL_FOUND AND PYMESH_USE_MKL)
    TARGET_LINK_LIBRARIES(SparseSolverDependencies
        INTERFACE
            PyMesh::MKL
    )
    TARGET_COMPILE_DEFINITIONS(SparseSolverDependencies
        INTERFACE
            -DWITH_MKL
    )
ENDIF (MKL_FOUND AND PYMESH_USE_MKL)

ADD_LIBRARY(lib_SparseSolver SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_SparseSolver PROPERTIES OUTPUT_NAME
    "PyMesh-SparseSolver")
TARGET_LINK_LIBRARIES(lib_SparseSolver
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        SparseSolverDependencies
)
