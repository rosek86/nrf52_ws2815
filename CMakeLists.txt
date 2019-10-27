cmake_minimum_required(VERSION 3.14)
project(nrf52_ws2815)
enable_language(C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

#[==[
  Custom targets section
#]==]

# Erase
add_custom_target(erase
  COMMAND ${CMAKE_COMMAND} -E echo "Erasing"
  COMMAND nrfjprog -f nrf52 --eraseall
)

# Readback protection
add_custom_target(rbp
  COMMENT "Setting up readback protection..."
  COMMAND nrfjprog -f nrf52 --rbp ALL
)

# Recover from readback protection
add_custom_target(recover
  COMMENT "Recovering device..."
  COMMAND nrfjprog -f nrf52 --recover
)
