cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW) # CMake 3.0
endif()
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW) # CMake 3.1
endif()

project (VolumeRendering)

find_package(VTK COMPONENTS
  vtkCommonCore
  vtkIOImage
  vtkIOXML
  vtkInteractionWidgets
  vtkRenderingVolume${VTK_RENDERING_BACKEND}
)
include(${VTK_USE_FILE})

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB INC "inc/*.h")

add_executable(VolumeRendering MACOSX_BUNDLE ${SRC} ${INC})
target_link_libraries(VolumeRendering ${VTK_LIBRARIES})


