message(STATUS "Settings main")

idf_component_register(SRCS "main.cpp" "calculator.cpp" "epdgui/epdgui_base.cpp" "epdgui/epdgui_button.cpp" "epdgui/epdgui_mutexswitch.cpp"  "epdgui/epdgui_switch.cpp" "epdgui/epdgui_textbox.cpp" "epdgui/epdgui.cpp"
                       INCLUDE_DIRS "." "./epdgui")

component_compile_options(
                -Wno-unused-const-variable
	            -Wno-missing-field-initializers)
