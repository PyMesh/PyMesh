# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(TetrahedralizationDependencies INTERFACE IMPORTED)

# Inlclude CGAL
IF(CGAL_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_CGAL
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DCGAL_CONCURRENT_MESH_3
    )
    ADD_SUBDIRECTORY(CGAL)
ENDIF(CGAL_FOUND)

# Include TetGen
IF(TETGEN_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_TetGen
    )
    ADD_SUBDIRECTORY(TetGen)
ENDIF(TETGEN_FOUND)

# Include Geogram
IF (GEOGRAM_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_Geogram
    )
    ADD_SUBDIRECTORY(Geogram)
ENDIF (GEOGRAM_FOUND)

# Include Quartet
IF (QUARTET_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            PyMesh::Quartet
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DWITH_QUARTET
    )
    ADD_SUBDIRECTORY(Quartet)
ENDIF (QUARTET_FOUND)

IF (MMG_FOUND AND TETGEN_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            PyMesh::MMG
            lib_IGL # For signed distance.
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DWITH_MMG
    )
    ADD_SUBDIRECTORY(MMG)
ENDIF (MMG_FOUND AND TETGEN_FOUND)

IF (TETWILD_FOUND)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            PyMesh::TetWild
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DWITH_TETWILD
    )
    ADD_SUBDIRECTORY(TetWild)
ENDIF (TETWILD_FOUND)

ADD_LIBRARY(lib_Tetrahedralization SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_Tetrahedralization
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        TetrahedralizationDependencies
)
SET_TARGET_PROPERTIES(lib_Tetrahedralization PROPERTIES OUTPUT_NAME
    "PyMesh-Tetrahedronization")

