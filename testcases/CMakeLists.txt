include_directories (${GTEST_INCLUDE_DIRS})
file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
add_executable (testcases ${src_files})
target_link_libraries (testcases ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
