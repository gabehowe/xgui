 cmake_minimum_required(VERSION 3.24)
 project(xgui C)

 set(CMAKE_C_STANDARD 11)
 set(WIN32 FALSE)
 set(CMAKE_PREFIX_PATH xgui/lib)

 find_package(Python3 COMPONENTS Interpreter Development)
 message(STATUS "Found python at ${Python3_INCLUDE_DIRS} ${Python3_LIBRARIES}")
 add_library(xgui SHARED xgui/guimodule.c)
 find_library(USER32_LIB User32)
 find_library(GDI32_LIB Gdi32)
 message(${USER32_LIB}\n ${GDI32_LIB})
 target_link_libraries(xgui ${Python3_LIBRARIES} ${USER32_LIB} ${GDI32_LIB})

 include_directories(${Python3_INCLUDE_DIRS} )
