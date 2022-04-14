# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)


set(CMAKE_TOOLCHAIN_FILE vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "CMake toolchain file" FORCE)

project ("ShapesRenderer")


# Include sub-projects.
add_subdirectory ("ShapesRenderer")

if(MSVC)
  target_compile_options(ShapesRenderer PUBLIC "/ZI")
  target_link_options(ShapesRenderer PUBLIC "/INCREMENTAL")
endif()
