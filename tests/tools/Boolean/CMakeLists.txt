# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Boolean_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Boolean_tests
    PRIVATE
        lib_Boolean
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_Boolean_tests
    DEPENDS
        Boolean_tests
    COMMAND
        Boolean_tests
)

# Cork
IF (CORK_FOUND)
    TARGET_COMPILE_DEFINITIONS(Boolean_tests
        PRIVATE
            -DWITH_CORK
    )
ENDIF (CORK_FOUND)

# Clipper
IF(CLIPPER_FOUND)
    TARGET_COMPILE_DEFINITIONS(Boolean_tests
        PRIVATE
            -DWITH_CLIPPER
    )
ENDIF(CLIPPER_FOUND)

# IGL
IF(LIBIGL_FOUND AND CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(Boolean_tests
        PRIVATE
            -DWITH_IGL
    )
ENDIF(LIBIGL_FOUND AND CGAL_FOUND)

# CGAL
IF (CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(Boolean_tests
        PRIVATE
            -DWITH_CGAL
    )
ENDIF (CGAL_FOUND)

# Carve
IF (CARVE_FOUND)
    TARGET_COMPILE_DEFINITIONS(Boolean_tests
        PRIVATE
            -DWITH_CARVE
    )
ENDIF (CARVE_FOUND)
