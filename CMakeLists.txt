 cmake_minimum_required(VERSION 3.24)
 project(cgui C)

 set(CMAKE_C_STANDARD 11)

 add_executable(cgui src/xgui/guimodule.c src/xgui/guimodulewrapped.c)
 target_include_directories(cgui PRIVATE "C:/Program Files/Python311/include")