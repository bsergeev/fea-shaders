cmake_minimum_required (VERSION 3.5)

if(NOT APP_NAME)
  set(APP_NAME ogl)
endif()
project(${APP_NAME} VERSION 0.0.1 LANGUAGES CXX)

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
)
message(STATUS "-> Sources: ${sources}")

find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)
if (NOT Qt5_FOUND)
  message(FATAL_ERROR "Qt5 not found.")
endif()
message(STATUS "-> Qt: ${Qt5_DIR}")

add_executable(${APP_NAME} ${sources} ${headers})

target_include_directories(${APP_NAME} 
  PRIVATE
    "${include_dirs}"
)

if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
endif()

if (WIN32 OR WIN64)
  target_link_libraries(${APP_NAME}  project_options
    Qt5::Widgets Qt5::OpenGL
    opengl32
  )
  get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            --verbose 0
            --no-compiler-runtime
            \"$<TARGET_FILE:${TARGET_NAME}>\"
    COMMENT "Deploying Qt...")
else() # Linux
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17") #  -stdlib=libc++
  target_link_libraries(${APP_NAME} project_options 
    Qt5::Widgets Qt5::OpenGL
    # c++ c++abi pthread
  )
endif()
