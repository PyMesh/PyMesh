# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *Test.h)

ADD_EXECUTABLE(SparseSolver_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(SparseSolver_tests
    PRIVATE
        lib_SparseSolver
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_SparseSolver_tests 
    DEPENDS
        SparseSolver_tests
    COMMAND
        SparseSolver_tests
)

IF (UMFPACK_FOUND)
    TARGET_COMPILE_DEFINITIONS(SparseSolver_tests
        PRIVATE
            -DWITH_UMFPACK
    )
ENDIF (UMFPACK_FOUND)

IF (CHOLMOD_FOUND)
    TARGET_COMPILE_DEFINITIONS(SparseSolver_tests
        PRIVATE
            -DWITH_CHOLMOD
    )
ENDIF (CHOLMOD_FOUND)

IF (MKL_FOUND)
    TARGET_COMPILE_DEFINITIONS(SparseSolver_tests
        PRIVATE
            -DWITH_MKL
    )
ENDIF (MKL_FOUND)

