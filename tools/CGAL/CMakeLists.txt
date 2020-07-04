# Enumerate source files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.h)

if (TARGET PyMesh::third_party::CGAL)
    add_library(lib_CGAL SHARED ${SRC_FILES} ${INC_FILES})
    set_target_properties(lib_CGAL PROPERTIES OUTPUT_NAME "PyMesh-CGAL")
    target_link_libraries(lib_CGAL
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
            PyMesh::third_party::CGAL
    )

    add_library(PyMesh::CGAL ALIAS lib_CGAL)
endif ()
