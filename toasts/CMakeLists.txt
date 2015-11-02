cmake_minimum_required(VERSION 2.8)

project(toasts)
add_compile_options(-std=c++11)
add_executable(toasts
  main.cpp
  processes/container.cpp
  processes/worker.cpp
  processes/toaster.cpp
  processes/butterer.cpp
  processes/jammer.cpp
  processes/controller.cpp
)
target_link_libraries(toasts ZThread)
install(TARGETS toasts RUNTIME DESTINATION bin)

add_subdirectory(processes)