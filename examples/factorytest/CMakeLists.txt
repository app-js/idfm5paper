message(STATUS "Settings main")

set(COMPONENT_SRCDIRS "." "epdgui" "frame")

set(COMPONENT_ADD_INCLUDEDIRS "resources")

register_component()

component_compile_options(
                -Wno-unused-const-variable
	            -Wno-missing-field-initializers)
