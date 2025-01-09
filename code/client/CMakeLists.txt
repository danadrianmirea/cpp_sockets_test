add_executable(client client.cpp)

# Ensure the bin directory is created
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET client POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:client> ${CMAKE_BINARY_DIR}/bin
  COMMENT "Copying client.exe to bin directory"
)