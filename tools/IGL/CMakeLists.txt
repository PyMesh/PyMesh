# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

if (TARGET PyMesh::third_party::libigl)
    add_library(lib_IGL SHARED ${SRC_FILES} ${INC_FILES})
    set_target_properties(lib_IGL PROPERTIES OUTPUT_NAME "PyMesh-IGL")
    target_link_libraries(lib_IGL
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
            PyMesh::MeshUtils
            PyMesh::third_party::libigl
    )

    if (TARGET PyMesh::CGAL)
        target_link_libraries(lib_IGL PUBLIC PyMesh::CGAL)
    endif()

    add_library(PyMesh::IGL ALIAS lib_IGL)
endif()
