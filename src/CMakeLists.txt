cmake_minimum_required(VERSION 3.14)
enable_language(C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(lint COMMAND cppcheck
  --enable=warning
  --project=compile_commands.json
  --quiet
)

set(APP_NAME nrf52_ws2815)

get_filename_component(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/${APP_NAME}.ld ABSOLUTE)
get_filename_component(SDK_ROOT ${CMAKE_CURRENT_LIST_DIR}/../support/sdk ABSOLUTE)
get_filename_component(SRC_DIR ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)

add_executable(${APP_NAME}
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_rtt.c
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_serial.c
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_uart.c
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_default_backends.c
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c
  ${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c
  ${SDK_ROOT}/components/libraries/util/app_error.c
  ${SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c
  ${SDK_ROOT}/components/libraries/util/app_error_weak.c
  ${SDK_ROOT}/components/libraries/util/app_util_platform.c
  ${SDK_ROOT}/components/libraries/util/nrf_assert.c
  ${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c
  ${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c
  ${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c
  ${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c
  ${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c
  ${SDK_ROOT}/components/boards/boards.c
  ${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c
  ${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c
  ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_i2s.c
  ${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c
  ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c
  ${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c
  ${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52840.S
  ${SDK_ROOT}/modules/nrfx/mdk/system_nrf52840.c
  ${SDK_ROOT}/external/fprintf/nrf_fprintf.c
  ${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c
  ${SDK_ROOT}/external/segger_rtt/SEGGER_RTT.c
  ${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
  ${SDK_ROOT}/external/segger_rtt/SEGGER_RTT_printf.c
  ${SRC_DIR}/drv_ws2815.c
  ${SRC_DIR}/main.c
 )

target_include_directories(${APP_NAME} PRIVATE
  ${SDK_ROOT}/components
  ${SDK_ROOT}/modules/nrfx/mdk
  ${SDK_ROOT}/components/libraries/strerror
  ${SDK_ROOT}/components/toolchain/cmsis/include
  ${SDK_ROOT}/components/libraries/util
  ${SDK_ROOT}/components/libraries/balloc
  ${SDK_ROOT}/components/libraries/ringbuf
  ${SDK_ROOT}/modules/nrfx/hal
  ${SDK_ROOT}/components/libraries/bsp
  ${SDK_ROOT}/components/libraries/log
  ${SDK_ROOT}/modules/nrfx
  ${SDK_ROOT}/components/libraries/experimental_section_vars
  ${SDK_ROOT}/integration/nrfx/legacy
  ${SDK_ROOT}/components/libraries/delay
  ${SDK_ROOT}/integration/nrfx
  ${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd
  ${SDK_ROOT}/external/segger_rtt
  ${SDK_ROOT}/components/boards
  ${SDK_ROOT}/components/libraries/memobj
  ${SDK_ROOT}/modules/nrfx/drivers/include
  ${SDK_ROOT}/components/libraries/log/src
  ${SDK_ROOT}/external/fprintf
  ${SDK_ROOT}/components/libraries/atomic
  ${SRC_DIR}
  ${SRC_DIR}/config
)

set_target_properties(${APP_NAME} PROPERTIES
  SUFFIX ".axf"
  C_STANDARD 11
  LINK_DEPENDS ${LINKER_SCRIPT}
)

# Optimization flags
target_compile_options(${APP_NAME} PRIVATE
  "$<IF:$<CONFIG:Debug>,-O0;-g3,-O3>"
)

target_compile_definitions(${APP_NAME} PRIVATE
  BOARD_PCA10056
  BSP_DEFINES_ONLY
  CONFIG_GPIO_AS_PINRESET
  FLOAT_ABI_HARD
  NRF52840_XXAA
  SWI_DISABLE0
  __HEAP_SIZE=8192
  __STACK_SIZE=8192
)

# C flags common to all targets
target_compile_options(${APP_NAME} PRIVATE -mcpu=cortex-m4 -mthumb -mabi=aapcs)
target_compile_options(${APP_NAME} PRIVATE -Wall -Werror)
target_compile_options(${APP_NAME} PRIVATE -mfloat-abi=hard -mfpu=fpv4-sp-d16)

# keep every function in a separate section, this allows linker to discard unused ones
target_compile_options(${APP_NAME} PRIVATE -ffunction-sections -fdata-sections -fno-strict-aliasing)
target_compile_options(${APP_NAME} PRIVATE -fno-builtin -fshort-enums)

# Linker flags

# Optimization flags
target_link_options(${APP_NAME} PRIVATE
  "$<IF:$<CONFIG:Debug>,-O0;-g3,-O3>"
)

target_link_options(${APP_NAME} PRIVATE
  -mthumb -mabi=aapcs -L${SDK_ROOT}/modules/nrfx/mdk -T${LINKER_SCRIPT}
)

target_link_options(${APP_NAME} PRIVATE -mcpu=cortex-m4)
target_link_options(${APP_NAME} PRIVATE -mfloat-abi=hard -mfpu=fpv4-sp-d16)

# generate map file
target_link_options(${APP_NAME} PRIVATE "LINKER:-Map=src/${APP_NAME}.map")

# let linker dump unused sections
target_link_options(${APP_NAME} PRIVATE "LINKER:--gc-sections")

# use newlib in nano version
target_link_options(${APP_NAME} PRIVATE --specs=nano.specs)

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
target_link_libraries(${APP_NAME} PRIVATE c nosys m)

add_custom_command(TARGET ${APP_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo ""
  COMMAND ${CMAKE_COMMAND} -E echo "Generating bin file..."
  COMMAND ${arm-none-eabi-objcopy} -O binary ${APP_NAME}.axf ${APP_NAME}.bin

  COMMAND ${CMAKE_COMMAND} -E echo ""
  COMMAND ${CMAKE_COMMAND} -E echo "Generating hex file..."
  COMMAND ${arm-none-eabi-objcopy} -O ihex ${APP_NAME}.axf ${APP_NAME}.hex

  # COMMAND ${CMAKE_COMMAND} -E echo ""
  # COMMAND ${CMAKE_COMMAND} -E echo "** Bootloader settings file **"
  # COMMAND nrfutil settings generate
  #   --family NRF52840
  #   --application ${APP_NAME}.hex
  #   --application-version 1
  #   --bootloader-version 2
  #   --bl-settings-version 2 settings.hex

  # COMMAND ${CMAKE_COMMAND} -E echo ""
  # COMMAND ${CMAKE_COMMAND} -E echo "** DFU file **"
  # COMMAND nrfutil pkg generate --hw-version 52 --sd-req 0x00
  #   --application ${APP_NAME}.hex ${APP_NAME}.zip
  #   --application-version 1
  #   --application-version-string "0.0.1"
  #   --key-file ${SRC_DIR}/../../keys/private.pem

  # COMMAND ${CMAKE_COMMAND} -E echo ""
  # COMMAND ${CMAKE_COMMAND} -E echo "** Merge all **"
  # COMMAND mergehex -m ./${APP_NAME}.hex ./settings.hex -o app_settings.hex
  # COMMAND mergehex -m ${SDK_ROOT}/components/softdevice/mbr/nrf52840/hex/mbr_nrf52_2.4.1_mbr.hex
  #   ../../bootloader/secure_bootloader.hex ./app_settings.hex -o ${APP_NAME}_bundle.hex

  COMMAND ${arm-none-eabi-size} ${APP_NAME}.axf
)

#[==[
  Custom targets section
#]==]

add_custom_target(flash_jlink
  COMMAND ${CMAKE_COMMAND} -E echo "Flashing bundle file..."
  COMMAND nrfjprog
          --family nrf52
          # --program ${APP_NAME}_bundle.hex
          --program ${APP_NAME}.hex
          --sectorerase
  COMMAND nrfjprog -f nrf52 --reset
)

# add_custom_target(flash_usb
#   COMMAND ${CMAKE_COMMAND} -E echo $ENV{SERIAL_PORT}
#   COMMAND nrfutil dfu usb-serial -pkg ${APP_NAME}.zip -b 115200 -p $ENV{SERIAL_PORT}
# )

# Edit SDK config
set(SDK_CONFIG_FILE ${SRC_DIR}/../config/sdk_config.h)
set(CMSIS_CONFIG_TOOL ${SDK_ROOT}/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar)
add_custom_target(app_sdk_config
  COMMAND java -jar ${CMSIS_CONFIG_TOOL} ${SDK_CONFIG_FILE}
)
