include(CMakeForceCompiler)

get_filename_component(COMPILER_PATH ${CMAKE_CURRENT_LIST_DIR}/../support/gcc-arm-none-eabi-7-2018-q2-update/bin/ ABSOLUTE)

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION   1)

# specify the cross compiler
set(CMAKE_C_COMPILER      ${COMPILER_PATH}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER    ${COMPILER_PATH}/arm-none-eabi-g++)

# Set other tools
set(arm-none-eabi-size    ${COMPILER_PATH}/arm-none-eabi-size)
set(arm-none-eabi-objcopy ${COMPILER_PATH}/arm-none-eabi-objcopy)
set(arm-none-eabi-objdump ${COMPILER_PATH}/arm-none-eabi-objdump)
set(arm-none-eabi-gdb     ${COMPILER_PATH}/arm-none-eabi-gdb)

set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Remove preset linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_ASM_FLAGS "")

# Set library options
set(SHARED_LIBS OFF)
set(STATIC_LIBS ON)
