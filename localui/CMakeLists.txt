# CMake for WWIV 5
cmake_minimum_required (VERSION 2.6)
project (localui)
include_directories(..)

set(COMMON_SOURCES
  colors.cpp
  curses_io.cpp
  curses_win.cpp
  input.cpp
  listbox.cpp
  )

if(UNIX) 
  find_package(Curses REQUIRED)
  message("Curses Library: ${CURSES_LIBRARIES}") 
endif()

add_library(localui ${COMMON_SOURCES} ${PLATFORM_SOURCES} ${CURSES_LIBRARIES})
target_link_libraries(localui ${CURSES_LIBRARIES})
