# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(WindingNumberDependencies INTERFACE IMPORTED)

# IGL and CGAL
IF (TARGET lib_IGL AND TARGET lib_CGAL AND PYMESH_USE_LIBIGL AND PYMESH_USE_CGAL)
    TARGET_LINK_LIBRARIES(WindingNumberDependencies
        INTERFACE
            lib_IGL
            lib_CGAL
    )
    ADD_SUBDIRECTORY(IGL)
ENDIF (TARGET lib_IGL AND TARGET lib_CGAL AND PYMESH_USE_LIBIGL AND PYMESH_USE_CGAL)

IF (FAST_WINDING_NUMBER_FOUND)
    TARGET_LINK_LIBRARIES(WindingNumberDependencies
        INTERFACE
            PyMesh::FAST_WINDING_NUMBER
    )
    TARGET_COMPILE_DEFINITIONS(WindingNumberDependencies
        INTERFACE
            -DWITH_FAST_WINDING_NUMBER
    )
    ADD_SUBDIRECTORY(FastWindingNumber)
ENDIF (FAST_WINDING_NUMBER_FOUND)

ADD_LIBRARY(lib_WindingNumber SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_WindingNumber PROPERTIES OUTPUT_NAME
    "PyMesh-WindingNumber")
TARGET_LINK_LIBRARIES(lib_WindingNumber
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        WindingNumberDependencies
)
