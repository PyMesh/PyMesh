# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(CGAL)

ADD_LIBRARY(lib_SkeletonExtraction SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_SkeletonExtraction
    PUBLIC
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        PyMesh::Tools::CGAL
)
SET_TARGET_PROPERTIES(lib_SkeletonExtraction PROPERTIES OUTPUT_NAME
    "PyMesh-SkeletonExtraction")

ADD_LIBRARY(PyMesh::Tools::SkeletonExtraction ALIAS lib_SkeletonExtraction)