
cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)
set(USER_AGENT ANON)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
#include(pico_extras_import_optional.cmake)

project(picow_button_discord C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

# Initialize the SDK
pico_sdk_init()

add_executable(picow_button_discord
        src/main.cpp
        src/tls.c
)

# For monitoring over usb.
# E.g. $ minicom -b 115200 -o -D /dev/ttyACM0
pico_enable_stdio_usb(picow_button_discord 1)
pico_enable_stdio_uart(picow_button_discord 0)

# These secrets are stored in plaintext...
target_compile_definitions(picow_button_discord PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        WEBHOOK=\"${WEBHOOK}\"
        USER_AGENT=\"${USER_AGENT}\"
        IMG=\"${IMG}\"
        ADC_VREF=${ADC_VREF}
        DEGREE_OFFSET=${DEGREE_OFFSET}
        ALTCP_MBEDTLS_AUTHMODE=MBEDTLS_SSL_VERIFY_REQUIRED
)

include_directories(include)

target_link_libraries(picow_button_discord
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mbedtls
        pico_mbedtls
        pico_stdlib
)

pico_add_extra_outputs(picow_button_discord)

# Ignore warnings from lwip code
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
)
