TEMPLATE = app
TARGET = simulate
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS+= -fopenmp -g
QMAKE_LFLAGS +=  -fopenmp -g

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

QMAKE_CXXFLAGS_DEBUG -= -O1
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_DEBUG -= -O3
QMAKE_CXXFLAGS_DEBUG *= -g

QMAKE_MAC_SDK = macosx10.11


SOURCES += main.cpp \
    Lattice/lattice.cpp \
    Node/node.cpp \
    NodeInfo/nodeinfo.cpp \
    Vec3/vec3.cpp \
    LatticeInfo/latticeinfo.cpp \
    ForceModifier/forcemodifier.cpp \
    ForceModifier/PotentialSurface/potentialsurface.cpp \
    ForceModifier/RelativeVelocityDamper/relativevelocitydamper.cpp \
    ForceModifier/PotentialPusher/potentialpusher.cpp \
    ForceModifier/ConstantMoment/constantmoment.cpp \
    ForceModifier/ConstantForce/constantforce.cpp \
    ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.cpp \
    ForceModifier/SpringFriction/springfriction.cpp \
    FrictionInfo/frictioninfo.cpp \
    DataOutput/datapacket.cpp \
    DataOutput/datapackethandler.cpp \
    DataOutput/dumpable.cpp \
    FrictionSystem/SidePotentialLoading/sidepotentialloading.cpp \
    InputManagement/Parameters/parameters.cpp \
    DriverBeam/driverbeam.cpp \
    Lattice/simplelattice.cpp \
    # ForceModifier/StraightenerForce/straightenerforce.cpp \
    Lattice/UnstructuredLattice/unstructuredlattice.cpp \
    InputManagement/LatticeScanner/latticescanner.cpp


HEADERS += \
    Lattice/lattice.h \
    Node/node.h \
    NodeInfo/nodeinfo.h \
    Vec3/vec3.h \
    LatticeInfo/latticeinfo.h \
    ForceModifier/forcemodifier.h \
    InputManagement/ConfigReader/configreader.h \
    InputManagement/Parameters/parameters.h \
    FrictionSystem/SidePotentialLoading/sidepotentialloading.h \
    FrictionInfo/frictioninfo.h \
    ForceModifier/PotentialPusher/potentialpusher.h \
    DataOutput/dumpable.h \
    DataOutput/datapackethandler.h \
    DataOutput/datapacket.h \
    ForceModifier/SpringFriction/springfriction.h \
    DriverBeam/driverbeam.h \
    Lattice/simplelattice.h \
    # ForceModifier/StraightenerForce/straightenerforce.h \
    Lattice/UnstructuredLattice/unstructuredlattice.h \
    InputManagment/LatticeScanner/latticescanner.h

LIBS += \
        -lboost_system\
        -lboost_filesystem\
        -lboost_program_options\
