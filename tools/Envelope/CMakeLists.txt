# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

#ADD_LIBRARY(lib_envelope SHARED ${SRC_FILES} ${INC_FILES})
#SET_TARGET_PROPERTIES(lib_envelope PROPERTIES OUTPUT_NAME "envelope")
#TARGET_LINK_LIBRARIES(lib_envelope Mesh lib_MeshUtils)

ADD_CUSTOM_TARGET(envelope DEPENDS)
