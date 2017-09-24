cmake_minimum_required(VERSION 2.8.12) 

if (CMAKE_COMPILER_IS_GNUCC)
  add_compile_options(-Wall -Wextra)
endif (CMAKE_COMPILER_IS_GNUCC)

add_library("elfload"
  "elf.h"
  "elfarch.h"
  "elfload.h"
  "elfload.c"
  "elfreloc_aarch64.c"
  "elfreloc_amd64.c"
  "elfreloc_i386.c")

add_executable("elfloader" "elfloader.c")
target_link_libraries("elfloader" "elfload")

add_executable("sample" "sample.c")

if (CMAKE_COMPILER_IS_GNUCC)
	set_target_properties("sample" PROPERTIES COMPILE_OPTIONS "-nostartfiles")
	set_target_properties("sample" PROPERTIES LINK_FLAGS "-nostdlib")
endif (CMAKE_COMPILER_IS_GNUCC)
