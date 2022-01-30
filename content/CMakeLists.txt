cmake_minimum_required(VERSION 3.2)
project(content NONE)

set(CONTENT_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/content")

if(WIIU_TEST_GX2)
   add_subdirectory(shaders)
endif()

install(DIRECTORY data DESTINATION "${CONTENT_INSTALL_DIR}")
