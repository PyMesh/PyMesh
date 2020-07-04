# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

if (TARGET PyMesh::third_party::Geogram)
    add_library(lib_Geogram SHARED ${SRC_FILES} ${INC_FILES})
    target_link_libraries(lib_Geogram
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
            PyMesh::third_party::Geogram
    )
    set_target_properties(lib_Geogram PROPERTIES OUTPUT_NAME "PyMesh-Geogram")
    add_library(PyMesh::Geogram ALIAS lib_Geogram)
endif ()
