# Enumerate source files
file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE INC_FILES *.h)

if (TARGET PyMesh::third_party::TetGen)
    add_library(lib_TetGen SHARED ${SRC_FILES} ${INC_FILES})
    set_target_properties(lib_TetGen PROPERTIES OUTPUT_NAME "PyMesh-TetGen")
    target_link_libraries(lib_TetGen
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
            PyMesh::third_party::TetGen
    )
    add_library(PyMesh::TetGen ALIAS lib_TetGen)
endif ()
