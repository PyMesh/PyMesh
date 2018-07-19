# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(TetrahedralizationDependencies INTERFACE IMPORTED)

# Inlclude CGAL
IF(CGAL_FOUND AND PYMESH_USE_CGAL)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_CGAL
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DCGAL_CONCURRENT_MESH_3
    )
    ADD_SUBDIRECTORY(CGAL)
ENDIF(CGAL_FOUND AND PYMESH_USE_CGAL)

# Include TetGen
IF(TETGEN_FOUND AND PYMESH_USE_TETGEN)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_TetGen
    )
    ADD_SUBDIRECTORY(TetGen)
ENDIF(TETGEN_FOUND AND PYMESH_USE_TETGEN)

# Include Geogram
IF (GEOGRAM_FOUND AND PYMESH_USE_GEOGRAM)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            lib_Geogram
    )
    ADD_SUBDIRECTORY(Geogram)
ENDIF (GEOGRAM_FOUND AND PYMESH_USE_GEOGRAM)

# Include Quartet
IF (QUARTET_FOUND AND PYMESH_USE_QUARTET)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            PyMesh::Quartet
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DWITH_QUARTET
    )
    ADD_SUBDIRECTORY(Quartet)
ENDIF (QUARTET_FOUND AND PYMESH_USE_QUARTET)

IF (MMG_FOUND AND TETGEN_FOUND AND PYMESH_USE_MMG AND PYMESH_USE_TETGEN)
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
ENDIF (MMG_FOUND AND TETGEN_FOUND AND PYMESH_USE_MMG AND PYMESH_USE_TETGEN)

IF (TETWILD_FOUND AND PYMESH_USE_TETWILD)
    TARGET_LINK_LIBRARIES(TetrahedralizationDependencies
        INTERFACE
            PyMesh::TetWild
    )
    TARGET_COMPILE_DEFINITIONS(TetrahedralizationDependencies
        INTERFACE
            -DWITH_TETWILD
    )
    ADD_SUBDIRECTORY(TetWild)
ENDIF (TETWILD_FOUND AND PYMESH_USE_TETWILD)

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

