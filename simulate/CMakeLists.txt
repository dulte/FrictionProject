cmake_minimum_required(VERSION 3.1)
project("Friction")

# Options
option(TEST "Build all tests." OFF)
option(CORES "Set the number of threads to use for multiprocessing" 4)
set(CORES 4)

add_definitions(-DNUM_THREADS=${CORES})

set(PROJECT_NAME Friction)

# Set the compiler flags for different build types
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -Wall -fopenmp -std=c++11 -D_GLIBCXX_PARALLEL -Wextra -Wfloat-equal -Wundef -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -march=native -Wswitch-enum -Wconversion -Wunreachable-code -Wformat=2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -Wall -fopenmp  -std=c++11 -D_GLIBCXX_PARALLEL -Wextra -Wshadow -Wfloat-equal -Wundef -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -march=native -Wswitch-enum -Wconversion -Wunreachable-code -Wformat=2")

# Set the files to include
set(INCLUDE_FILES
    src/main.cpp
    src/Lattice/lattice.cpp
    src/Node/node.cpp
    src/NodeInfo/nodeinfo.cpp
    src/Vec3/vec3.cpp
    src/LatticeInfo/latticeinfo.cpp
    src/ForceModifier/forcemodifier.cpp
    src/ForceModifier/PotentialSurface/potentialsurface.cpp
    src/ForceModifier/RelativeVelocityDamper/relativevelocitydamper.cpp
    src/ForceModifier/PotentialPusher/potentialpusher.cpp
    src/ForceModifier/ConstantMoment/constantmoment.cpp
    src/ForceModifier/ConstantForce/constantforce.cpp
    src/ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.cpp
    src/ForceModifier/SpringFriction/springfriction.cpp
    src/FrictionInfo/frictioninfo.cpp
    src/DataOutput/datapacket.cpp
    src/DataOutput/datapackethandler.cpp
    src/DataOutput/dumpable.cpp
    src/DataOutput/filewrapper.cpp
    src/DataOutput/mkdir.h
    src/DataOutput/DataDumper/datadumper.cpp
    src/FrictionSystem/frictionsystem.cpp
    src/FrictionSystem/TopPotentialLoading/toppotentialloading.cpp
    src/FrictionSystem/SidePotentialLoading/sidepotentialloading.cpp
    src/FrictionSystem/Cantilever/cantilever.cpp
    src/FrictionSystem/BulkWave/bulkwave.cpp
    src/FrictionSystem/BulkStretch/bulkstretch.cpp
    src/FrictionSystem/Rotate/rotate.cpp
    src/InputManagement/Parameters/parameters.cpp
    src/DriverBeam/driverbeam.cpp
    src/Lattice/TriangularLattice/triangularlattice.cpp
    src/Lattice/UnstructuredLattice/unstructuredlattice.cpp
    src/InputManagement/LatticeScanner/latticescanner.cpp
    src/InputManagement/Parameters/parameter.cpp
    src/Simulation/simulation.cpp
    )
set(EXECUTABLE_NAME simulate)

# Set the default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RELEASE")
endif(NOT CMAKE_BUILD_TYPE)
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# Libraries
include_directories("src")
add_library(lfriction ${INCLUDE_FILES})
add_executable(${EXECUTABLE_NAME} ${INCLUDE_FILES})
target_link_libraries(${EXECUTABLE_NAME} lfriction)

# Testing. Under construction
if (TEST)
  if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
  endif (APPLE)

  # This adds another subdirectory, which has 'project(googletest)'.
  add_subdirectory(../../googletest gtest)

  enable_testing()

  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ##############
  # Unit Tests
  ##############
  add_executable(runUnitTests test/test.cpp)

  # Standard linking to gtest stuff.
  target_link_libraries(runUnitTests gtest gtest_main)

  # # Extra linking for the project.
  target_link_libraries(runUnitTests project_lib blas lapack -O1 armadillo -ljsoncpp)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  # add_test(NAME that-test-I-made COMMAND runUnitTests)

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
  # add_test(that-other-test-I-made runUnitTests)
  add_test(MetamodelTest runUnitTests)
endif()
