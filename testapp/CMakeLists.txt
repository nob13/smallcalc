# Main Executable
set (files "main.cpp")
add_executable (testapp ${files})
target_link_libraries (testapp ${LIBS})

install (TARGETS  testapp RUNTIME DESTINATION bin)
