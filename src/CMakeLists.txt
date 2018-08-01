# Enumerate source files
FILE(GLOB_RECURSE SRC_FILES *.cpp *.c)
FILE(GLOB_RECURSE INC_FILES */*.h)

# Add executable and library
ADD_LIBRARY(Mesh SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(Mesh PROPERTIES OUTPUT_NAME "PyMesh-Mesh")
TARGET_LINK_LIBRARIES(Mesh
    PUBLIC
        PyMesh::Eigen
        PyMesh::TBB
)
TARGET_INCLUDE_DIRECTORIES(Mesh SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
# Google sparse/dense hash
IF (SPARSEHASH_FOUND)
    TARGET_LINK_LIBRARIES(Mesh
        PRIVATE
            PyMesh::SparseHash
    )
ENDIF (SPARSEHASH_FOUND)

