# CMakeList.txt : CMake project for ShapesRenderer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ShapesRenderer "ShapesRenderer.cpp" "ShapesRenderer.h" "Camera.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShapesRenderer PROPERTY CXX_STANDARD 20)
endif()

 
     find_package(glad CONFIG REQUIRED)
     target_link_libraries(ShapesRenderer PRIVATE glad::glad)
 
     find_package(glm CONFIG REQUIRED)
     target_link_libraries(ShapesRenderer PRIVATE glm::glm)
 
     find_package(SDL2 CONFIG REQUIRED)
     target_link_libraries(ShapesRenderer PRIVATE SDL2::SDL2 SDL2::SDL2main opengl32 glu32)

      find_package(imgui CONFIG REQUIRED)
      target_link_libraries(ShapesRenderer PRIVATE imgui::imgui)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZI /EDITANDCONTINUE")

# TODO: Add tests and install targets if needed.
