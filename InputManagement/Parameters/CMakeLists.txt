project("ParameterTest")
# FIND_PACKAGE( Boost 1.60 COMPONENTS system filesystem program_options REQUIRED )
# INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
include_directories(".")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -fopenmp -std=c++11")
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
add_executable(params main.cpp
    parameters.cpp
    parameter.cpp
  )
# TARGET_LINK_LIBRARIES( simulate LINK_PUBLIC ${Boost_LIBRARIES} )
