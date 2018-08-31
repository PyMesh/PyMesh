# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Compression_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Compression_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Compression
        PyMesh::Draco
)
ADD_CUSTOM_TARGET(run_Compression_tests
    DEPENDS
        Compression_tests
    COMMAND 
        Compression_tests
)

