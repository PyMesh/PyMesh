# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

#ADD_LIBRARY(lib_envelope SHARED ${SRC_FILES} ${INC_FILES})
#SET_TARGET_PROPERTIES(lib_envelope PROPERTIES OUTPUT_NAME "envelope")
#TARGET_LINK_LIBRARIES(lib_envelope Mesh lib_MeshUtils)

IF(NOT WITHOUT_SWIG)
    set(SWG_FILES PyEnvelope.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyEnvelope python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyEnvelope ${PYTHON_LIBRARIES} Mesh lib_MeshUtils)

    # Boudle lib, swig wrapper into a single target.
    # Usage: make Envelope
    ADD_CUSTOM_TARGET(envelope DEPENDS _PyEnvelope)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(envelope DEPENDS)
ENDIF(NOT WITHOUT_SWIG)
