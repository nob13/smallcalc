# Main Library
file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} smallcalc/*.h)

if (ANDROID)
	set (LIBRARY_TYPE SHARED)
else()
	set (LIBRARY_TYPE STATIC)
endif()

add_library (smallcalc ${LIBRARY_TYPE} ${src_files} ${header_files})

# Install Header Files
install (DIRECTORY smallcalc DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "smallcalc/impl*" EXCLUDE)

install (TARGETS  smallcalc ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
if (IOS)
	# Workaround Bug http://code.google.com/p/ios-cmake/issues/detail?id=6
	install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION lib COMPONENT smallcalc FILES_MATCHING PATTERN "*.a")
endif()
