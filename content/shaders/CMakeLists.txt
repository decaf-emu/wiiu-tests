cmake_minimum_required(VERSION 3.2)
project(shaders NONE)

find_program(AMD_SHADER_ANALYZER GPUShaderAnalyzer)
find_program(LATTE_ASSEMBLER latte-assembler)

if(NOT LATTE_ASSEMBLER)
   message(FATAL_ERROR "latte-assembler is required to build shaders")
endif()

if(NOT AMD_SHADER_ANALYZER)
   message(FATAL_ERROR "GPUShaderAnalyzer is required to build shaders")
endif()

macro(assemble_shader target vsh psh)
   add_custom_target(
      ${target} ALL
      ${LATTE_ASSEMBLER} assemble
         --psh "${PROJECT_SOURCE_DIR}/${psh}"
         --vsh "${PROJECT_SOURCE_DIR}/${vsh}"
         "${CMAKE_CURRENT_BINARY_DIR}/${target}.gsh"
      BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${target}.gsh
      DEPENDS ${psh} ${vsh}
      COMMENT "Assembling ${target}.gsh")

   install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.gsh" DESTINATION "${CONTENT_INSTALL_DIR}/shaders")
endmacro()

macro(compile_shader target vsh psh)
   add_custom_target(
      ${target} ALL
      ${LATTE_ASSEMBLER} compile
         --amd-shader-analyzer "${AMD_SHADER_ANALYZER}"
         --psh "${PROJECT_SOURCE_DIR}/${psh}"
         --vsh "${PROJECT_SOURCE_DIR}/${vsh}"
         "${CMAKE_CURRENT_BINARY_DIR}/${target}.gsh"
      BYPRODUCTS ${target}.gsh
      DEPENDS ${psh} ${vsh}
      COMMENT "Compiling ${target}.gsh")

   install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.gsh" DESTINATION "${CONTENT_INSTALL_DIR}/shaders")
endmacro()

assemble_shader(pos_colour pos_colour.vsh pos_colour.psh)
compile_shader(pos_colour_mvp pos_colour_mvp.vert.glsl pos_colour_mvp.frag.glsl)
