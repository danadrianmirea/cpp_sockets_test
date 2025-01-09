add_executable(server server.cpp)

# Ensure the bin directory is created
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add post-build steps to copy executables to the bin directory
add_custom_command(TARGET server POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:server> ${CMAKE_BINARY_DIR}/bin
  COMMENT "Copying server.exe to bin directory"
)