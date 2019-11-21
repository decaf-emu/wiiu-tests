cmake_minimum_required(VERSION 3.2)
include($ENV{DEVKITPRO}/wut/share/wut.cmake REQUIRED)
project(wiiu-tests C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(common)

macro(add_rpx source)
   get_filename_component(target ${source} NAME_WE)
   add_executable(${target} ${source})
   target_link_libraries(${target} m)
   wut_create_rpx(${target}.rpx ${target})
   install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.rpx" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endmacro()

add_subdirectory(coreinit)
add_subdirectory(gx2)
