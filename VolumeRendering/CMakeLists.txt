cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW) # CMake 3.0
endif()
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW) # CMake 3.1
endif()

project (VolumeRendering)

#find_package(VTK COMPONENTS
#  vtkCommonCore
#  vtkIOImage
#  vtkIOXML
#  vtkInteractionWidgets
#  vtkRenderingVolume${VTK_RENDERING_BACKEND}
#)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt5Widgets REQUIRED)
#include(${QT_USE_FILE})

include_directories(
  "${PROJECT_SOURCE_DIR}/inc"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB INC "inc/*.h")
FILE(GLOB UI_FILES "gui/*.ui")

qt5_wrap_ui(UISrcs ${UI_FILES})
qt5_wrap_cpp(MOCSrcs ${INC})

add_executable(VolumeRendering MACOSX_BUNDLE ${SRC} ${INC} ${UISrcs} ${MOCSrcs})
qt5_use_modules(VolumeRendering Core Gui)

if(VTK_LIBRARIES)
  target_link_libraries(VolumeRendering ${VTK_LIBRARIES})
else()
  target_link_libraries(VolumeRendering vtkHybrid QVTK vtkWidgets ${QT_LIBRARIES})
endif()


