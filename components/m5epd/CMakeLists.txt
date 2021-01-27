set(IDF_PATH_CMAKE $ENV{IDF_PATH})

string(REPLACE "\\" "/" IDF_PATH_CMAKE "${IDF_PATH_CMAKE}")

set(COMPONENT_SRCDIRS "src" "src/utility" "src/Fonts" "src/base" "src/sfnt" "src/smooth" "src/truetype")

set(COMPONENT_ADD_INCLUDEDIRS 
        "src" 
        "${IDF_PATH_CMAKE}/components/bt/host/bluedroid/api/include/api" 
        "${IDF_PATH_CMAKE}/components/wifi_provisioning/include" 
        "${IDF_PATH_CMAKE}/components/protocomm/include/common"
        "${IDF_PATH_CMAKE}/components/protocomm/include/security"
        "../arduino/tools/sdk/esp32/include/soc/esp32/include/soc")

list(APPEND COMPONENT_REQUIRES "arduino")

set(CONFIG_BT_BLUEDROID_ENABLED 1)

register_component()


set(DEFINITIONS "ESP32")

list(APPEND DEFINITIONS "M5STACK")

list(APPEND DEFINITIONS "M5STACK_FIRE")

target_compile_definitions(${COMPONENT_TARGET} PUBLIC "-DESP32")
