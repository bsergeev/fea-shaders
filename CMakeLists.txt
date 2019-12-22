cmake_minimum_required (VERSION 3.5)

if(NOT TARGET_NAME)
  set(TARGET_NAME ogl)
endif()
project(${TARGET_NAME} VERSION 0.0.1 LANGUAGES CXX)

# Define project_options "library" with all the build options and warnings
include(cmake/ProjectOptions.cmake)
include(cmake/BuildType.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(include_dirs  "${PROJECT_SOURCE_DIR}/src")
set(headers
  src/geometryengine.h
  src/mainwidget.h
)
set(sources
  src/geometryengine.cpp
  src/main.cpp
  src/mainwidget.cpp
  
  src/shaders.qrc
  src/textures.qrc
  src/fshader.glsl
  src/vshader.glsl
)
message(STATUS "-> Sources: ${sources}")

find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
if (NOT Qt5_FOUND)
  message(FATAL_ERROR "Qt5 not found.")
endif()
message(STATUS "-> Qt: ${Qt5_DIR}")

add_executable(${TARGET_NAME} ${sources} ${headers})

target_include_directories(${TARGET_NAME} 
  PRIVATE
    "${include_dirs}"
)

if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
endif()

if (WIN32 OR WIN64)
  target_link_libraries(${TARGET_NAME}  project_options
    Qt5::Widgets Qt5::OpenGL
    opengl32
  )
  #-----------------------------------------------------------------------------
  # Everything below is merely for copying Qt DLLs to the EXE location
  set(QT_BIN_LOCATION "${Qt5_DIR}/../../../bin")
  set(USED_QT_MODULES  Core Gui Widgets OpenGL)
  set (CustomCommands)
  macro(add_copy_commands _file_list)
    set(_destination $<TARGET_FILE_DIR:${TARGET_NAME}>)
    foreach(_fn ${${_file_list}})
      set(COMMAND ${ARGN} ${CMAKE_COMMAND} -E copy_if_different ${_fn} ${_destination} "\n")
      list(APPEND CustomCommands ${COMMAND})
    endforeach()
  endmacro()
  
  # Qt files based on the list of modules used are placed in the target directory.
  set(Debug_Resources)
  set(Optimized_Resources)
  foreach (qtlib ${USED_QT_MODULES})
    list(APPEND Debug_Resources     ${QT_BIN_LOCATION}/Qt5${qtlib}d.dll)
    list(APPEND Optimized_Resources ${QT_BIN_LOCATION}/Qt5${qtlib}.dll)
  endforeach(qtlib)
  
  add_copy_commands(Debug_Resources     if $<CONFIGURATION> == Debug)
  add_copy_commands(Optimized_Resources if $<CONFIGURATION> == Release)
  
  # Finally create the custom command as a post-build step.
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CustomCommands})
  #-----------------------------------------------------------------------------
else() # Linux
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17") #  -stdlib=libc++
  target_link_libraries(${TARGET_NAME} project_options 
    Qt5::Widgets Qt5::OpenGL
    # c++ c++abi pthread
  )
endif()
