project("Converter")
include_directories("..")
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -fopenmp -std=c++14")
add_executable(convert main.cpp
    converter.cpp
    ../InputManagement/Parameters/parameters.cpp
  )
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
