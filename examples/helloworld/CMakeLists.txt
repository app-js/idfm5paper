message(STATUS "Settings main")

idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS ".")

component_compile_options(
        -Wno-unused-const-variable
	    -Wno-missing-field-initializers)
