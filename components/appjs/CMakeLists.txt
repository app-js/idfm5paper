message(STATUS "Settings APPJS")

set(COMPONENT_SRCDIRS "src")

set(COMPONENT_ADD_INCLUDEDIRS "src")

list(APPEND COMPONENT_REQUIRES "arduino")
list(APPEND COMPONENT_REQUIRES "nvs_flash")
list(APPEND COMPONENT_REQUIRES "json")

register_component()

component_compile_options(
                -Wno-unused-but-set-parameter
	            -Wno-missing-field-initializers)
