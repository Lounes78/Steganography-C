cmake_minimum_required(VERSION 3.20)
project(test_png)
set(CMAKE_C_STANDARD 99)

set(LIBDEPS "${CMAKE_SOURCE_DIR}/deps")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

file(COPY "${CMAKE_SOURCE_DIR}/lena.png" DESTINATION "${CMAKE_BINARY_DIR}")

# Find the libpng library
#find_package(PNG REQUIRED)

include_directories( "${LIBDEPS}/include")

# Add an executable target
add_executable(test_png pngtest.c pngwrap.c)

# Link the libpng library to the test_png target
#target_link_libraries(test_png PNG::PNG)
target_link_libraries(test_png "${LIBDEPS}/lib/libpng16_static.lib" "${LIBDEPS}/lib/zlibstatic.lib")
