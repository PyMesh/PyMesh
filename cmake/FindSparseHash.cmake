# Find SparseHash (https://code.google.com/p/sparsehash/)
# The following variables are set.
#
#   SPARSEHASH_FOUND
#   SPARSEHASH_INCLUDE_DIRS
#
# and the following target:
#
#   SPARSEHASH

# Find include directory
FIND_PATH(SPARSEHASH_INCLUDE_DIRS sparsehash/sparse_hash_map
    $ENV{SPARSEHASH_PATH}
    $ENV{SPARSEHASH_PATH}/include
    /opt/local/include
    /usr/local/include
    /usr/include)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SparseHash
    "SparseHash not found, consider set $SPARSEHASH_PATH environment variable"
    SPARSEHASH_INCLUDE_DIRS)

MARK_AS_ADVANCED(SPARSEHASH_INCLUDE_DIRS)

IF (SPARSEHASH_FOUND AND NOT TARGET PyMesh::SparseHash)
    ADD_LIBRARY(PyMesh::SparseHash INTERFACE IMPORTED)
    TARGET_INCLUDE_DIRECTORIES(PyMesh::SparseHash SYSTEM
        INTERFACE
            ${SPARSEHASH_INCLUDE_DIRS}
    )
    TARGET_COMPILE_DEFINITIONS(PyMesh::SparseHash
        INTERFACE
            -DUSE_SPARSEHASH
    )
ENDIF (SPARSEHASH_FOUND AND NOT TARGET PyMesh::SparseHash)
