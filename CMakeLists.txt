cmake_minimum_required(VERSION 3.2)
project(wiiu-tests NONE)

option(WIIU_TEST_COREINIT    "Build tests for coreinit"    ON)
option(WIIU_TEST_GX2         "Build tests for gx2"         ON)

add_subdirectory(content)
add_subdirectory(src)
