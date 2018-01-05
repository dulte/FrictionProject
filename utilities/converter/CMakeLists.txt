project("Converter")
include_directories("src")
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -fopenmp -std=c++14")
add_executable(convert src/main.cpp
    src/converter.cpp
    ../simulate/src/InputManagement/Parameters/parameters.cpp
  )
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
