set(SRC_FILES TriangleWrapper.cpp)
set(INC_FILES TriangleWrapper.h)

if (TARGET PyMesh::third_party::Triangle)
    add_library(lib_Triangle SHARED ${SRC_FILES} ${INC_FILES})
    set_target_properties(lib_Triangle PROPERTIES OUTPUT_NAME "PyMesh-Triangle")
    target_link_libraries(lib_Triangle 
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
            PyMesh::third_party::Triangle
        PRIVATE
            PyMesh::MeshUtils
    )
    add_library(PyMesh::Triangle ALIAS lib_Triangle)
endif ()
