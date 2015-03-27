ADD_EXECUTABLE(inflate inflate.cpp)
TARGET_LINK_LIBRARIES(inflate Mesh lib_wires)

ADD_CUSTOM_TARGET(wires_examples DEPENDS inflate)
