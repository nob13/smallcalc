cmake_minimum_required (VERSION 2.6)
project (libsmallcalc)
enable_testing ()
# set (CMAKE_VERBOSE_MAKEFILE ON)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "DEBUG")
endif()

if (WIN32)
	add_definitions("-DWIN32")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS") # otherwise windows doesn't like fopen etc.
endif()

if (NOT WIN32)
	# Warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

if (DEFINED BOOST_ROOT)
	# find_package doesn't work on my mac building iOS builds
	message(STATUS "BOOST_ROOT defined, taking it, ${BOOST_ROOT}/include")
	include_directories (${BOOST_ROOT}/include)
else()
	find_package (Boost 1.40.0 REQUIRED)
	include_directories (${Boost_INCLUDE_DIRS})
	message (STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
endif()

if (ANDROID)
    message (STATUS "Hello Android")
    message (STATUS "ANDROID_NATIVE_API_LEVEL: ${ANDROID_NATIVE_API_LEVEL}")
    # Debugging Symbols    
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    # Fix Boost for Android
    add_definitions ("-DBOOST_DETAIL_ENDIAN_HPP -DBOOST_LITTLE_ENDIAN -DBOOST_BYTE_ORDER=1234")
endif()

if (IOS)
    message (STATUS "Hello iOS")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DNDEBUG")
endif()

message (STATUS "CXX Flags:        ${CMAKE_CXX_FLAGS}")
message (STATUS "CXX Release Flags ${CMAKE_CXX_FLAGS_RELEASE}")
message (STATUS "CXX Debug Flags   ${CMAKE_CXX_FLAGS_DEBUG}")
message (STATUS "Install Prefix:   ${CMAKE_INSTALL_PREFIX}") 

set (LIBS "")

# libsmallcalc library
add_subdirectory (libsmallcalc)
set (LIBS ${LIBS} "smallcalc")
include_directories ("libsmallcalc")

# Executeable
if (NOT IOS)
    add_subdirectory (testapp)
endif()

# Testcases
if (NOT IOS AND NOT ANDROID)
	find_package (GTest)
	if (GTEST_FOUND)
		if (NOT WIN32)
			# gtest on fedora is broken
			set (GTEST_LIBRARIES ${GTEST_LIBRARIES} pthread)
		endif()
	else()
		message (STATUS "GTest not found, using shipped variant")
		set(GTEST_DIRECTORY "dependencies_source/gtest-1.7.0")
		add_subdirectory(${GTEST_DIRECTORY})
		include_directories(${GTEST_DIRECTORY}/include)
		set (GTEST_LIBRARIES gtest)
		set (GTEST_MAIN_LIBRARIES gtest_main)
	endif()
	add_subdirectory(testcases)
endif()


