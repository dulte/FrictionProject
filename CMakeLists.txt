project("Friction")
# FIND_PACKAGE( Boost 1.60 COMPONENTS system filesystem program_options REQUIRED )
# INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
include_directories(".")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -fopenmp -std=c++11")
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
add_executable(simulate main.cpp
    Lattice/lattice.cpp
    Node/node.cpp
    NodeInfo/nodeinfo.cpp
    Vec3/vec3.cpp
    LatticeInfo/latticeinfo.cpp
    ForceModifier/forcemodifier.cpp
    ForceModifier/PotentialSurface/potentialsurface.cpp
    ForceModifier/RelativeVelocityDamper/relativevelocitydamper.cpp
    ForceModifier/PotentialPusher/potentialpusher.cpp
    ForceModifier/ConstantMoment/constantmoment.cpp
    ForceModifier/ConstantForce/constantforce.cpp
    ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.cpp
    ForceModifier/SpringFriction/springfriction.cpp
    FrictionInfo/frictioninfo.cpp
    DataOutput/datapacket.cpp
    DataOutput/datapackethandler.cpp
    DataOutput/dumpable.cpp
    FrictionSystem/SidePotentialLoading/sidepotentialloading.cpp
    InputManagement/Parameters/parameters.cpp
    DriverBeam/driverbeam.cpp
    Lattice/simplelattice.cpp
    Lattice/UnstructuredLattice/unstructuredlattice.cpp
    InputManagement/LatticeScanner/latticescanner.cpp
    InputManagement/Parameters/parameter.cpp
  )
# TARGET_LINK_LIBRARIES( simulate LINK_PUBLIC ${Boost_LIBRARIES} )
