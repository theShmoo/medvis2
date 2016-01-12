cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW) # CMake 3.0
endif()
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW) # CMake 3.1
endif()

project (VolumeRendering)

set(KIT VolumeRendering)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt5Widgets REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(
  ${OPENGL_INCLUDE_DIR}
)

set(KIT_GLSL_SRCS
   CompositeFS
   OneComponentFS
   FourComponentsFS
   NoShadeFS
   ShadeFS
   vtkGPUVolumeRayCastMapper_HeaderFS
   vtkGPUVolumeRayCastMapper_ParallelProjectionFS
   vtkGPUVolumeRayCastMapper_CompositeNoCroppingFS
   vtkGPUVolumeRayCastMapper_PerspectiveProjectionFS
   vtkGPUVolumeRayCastMapper_CompositeCroppingFS
   PostProcessGradientFilter
)

# Needs vtkEncodeString, VTK_ENCODESTRING_EXE
# VTK_VOLUMERENDERING_EXPORT is the flag associated to the library, should be adapted
foreach(file ${KIT_GLSL_SRCS})
  set(src  ${PROJECT_SOURCE_DIR}/GLSL/${file}.glsl)
  set(res  ${PROJECT_SOURCE_DIR}/src/${file}.cxx)
  set(resh ${PROJECT_SOURCE_DIR}/inc/${file}.h)
  add_custom_command(
    OUTPUT ${res} ${resh}
    DEPENDS ${src} vtkEncodeString
    COMMAND ${VTK_ENCODESTRING_EXE}
    ARGS ${res} ${src} ${file}
    )
  set(Kit_EXTRA_SRCS ${Kit_EXTRA_SRCS} ${res})
endforeach(file)

include_directories(
  "${PROJECT_SOURCE_DIR}/inc"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB INC "inc/*.h")
FILE(GLOB UI_FILES "gui/*.ui")

qt5_wrap_ui(UISrcs ${UI_FILES})
qt5_wrap_cpp(MOCSrcs ${INC})

add_executable(VolumeRendering MACOSX_BUNDLE ${SRC} ${INC} ${UISrcs} ${MOCSrcs} ${Kit_EXTRA_SRCS})
qt5_use_modules(VolumeRendering Core Gui Xml)

if(VTK_LIBRARIES)
  target_link_libraries(VolumeRendering ${VTK_LIBRARIES} ${OPENGL_gl_LIBRARY} ${SSC_GCOV_LIBRARY})
else()
  target_link_libraries(VolumeRendering vtkHybrid QVTK vtkWidgets vtkVolumeRendering vtkIO vtkRendering ${QT_LIBRARIES} ${OPENGL_gl_LIBRARY} ${SSC_GCOV_LIBRARY})
endif()


