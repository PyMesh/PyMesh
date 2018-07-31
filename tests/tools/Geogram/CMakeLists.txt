# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Geogram_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Geogram_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Geogram
)
ADD_CUSTOM_TARGET(run_Geogram_tests
    DEPENDS
        Geogram_tests
    COMMAND
        Geogram_tests
)
