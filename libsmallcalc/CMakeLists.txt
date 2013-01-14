# Main Library
file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.h)

if (ANDROID)
	set (LIBRARY_TYPE SHARED)
else()
	set (LIBRARY_TYPE STATIC)
endif()

add_library (smallcalc ${LIBRARY_TYPE} ${src_files} ${header_files})

install (FILES    ${header_files} DESTINATION include/)
install (TARGETS  smallcalc ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
