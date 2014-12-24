# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_EigenUtils SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_EigenUtils PROPERTIES OUTPUT_NAME "EigenUtiles")
TARGET_LINK_LIBRARIES(lib_EigenUtils Mesh)

IF(NOT WITHOUT_SWIG)
    set(SWG_FILES PyEigenUtils.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyEigenUtils python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyEigenUtils ${PYTHON_LIBRARIES} lib_EigenUtils Mesh)

    # Boudle lib, swig wrapper into a single target.
    # Usage: make EigenUtils
    ADD_CUSTOM_TARGET(EigenUtils DEPENDS lib_EigenUtils _PyEigenUtils)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(EigenUtils DEPENDS lib_EigenUtils)
ENDIF(NOT WITHOUT_SWIG)
