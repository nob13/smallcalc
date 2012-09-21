# Main Library
file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.h)

add_library (smallcalc ${src_files} ${header_files})

install (FILES    ${header_files} DESTINATION include/)
install (TARGETS  smallcalc ARCHIVE DESTINATION lib)
