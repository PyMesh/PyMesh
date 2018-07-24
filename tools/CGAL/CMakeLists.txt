IF(CGAL_FOUND AND PYMESH_USE_CGAL)
    # Enumerate source files
    FILE(GLOB SRC_FILES *.cpp)
    FILE(GLOB INC_FILES *.h)

    ADD_LIBRARY(lib_CGAL SHARED ${SRC_FILES} ${INC_FILES})
    SET_TARGET_PROPERTIES(lib_CGAL PROPERTIES OUTPUT_NAME "PyMesh-CGAL")
    TARGET_LINK_LIBRARIES(lib_CGAL
        PUBLIC
            ${CGAL_LIBRARIES}
            ${Boost_LIBRARIES}
            ${CGAL_3RD_PARTY_LIBRARIES}
            ${GMP_LIBRARIES}
            ${MPFR_LIBRARIES}
            Mesh
            PyMesh::Tools
    )
    TARGET_INCLUDE_DIRECTORIES(lib_CGAL SYSTEM
        PUBLIC
            ${CGAL_INCLUDE_DIRS}
            ${CGAL_3RD_PARTY_INCLUDE_DIRS}
            ${GMP_INCLUDE_DIRS}
            ${MPFR_INCLUDE_DIRS}
    )
    TARGET_COMPILE_DEFINITIONS(lib_CGAL
        PUBLIC
            ${CGAL_3RD_PARTY_DEFINITIONS}
            ${CGAL_DEFINITIONS}
    )

    IF (CGAL_HEADER_ONLY)
        # It seems CGAL does not capture this flag in ${CGAL_DEFINITIONS}...
        TARGET_COMPILE_DEFINITIONS(lib_CGAL
            PUBLIC
                -DCGAL_HEADER_ONLY
        )
    ENDIF (CGAL_HEADER_ONLY)
    TARGET_COMPILE_DEFINITIONS(lib_CGAL
        INTERFACE
            -DWITH_CGAL
    )
ENDIF(CGAL_FOUND AND PYMESH_USE_CGAL)
