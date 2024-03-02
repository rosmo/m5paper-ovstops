message(STATUS "Settings main")

set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

list(APPEND COMPONENT_REQUIRES "nvs_flash" "m5epd_lvgl" "spi_flash" "esp_wifi" "esp_http_client" "json" "mbedtls")

register_component()

component_compile_options(
        -Wno-unused-const-variable
	-Wno-missing-field-initializers
        -Wno-format)
