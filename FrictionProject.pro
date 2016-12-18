TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Lattice/lattice.cpp \
    Node/node.cpp \
    NodeInfo/nodeinfo.cpp \
    Vec3/vec3.cpp \
    LatticeInfo/latticeinfo.cpp \
    ForceModifier/forcemodifier.cpp

HEADERS += \
    Lattice/lattice.h \
    Node/node.h \
    NodeInfo/nodeinfo.h \
    Vec3/vec3.h \
    LatticeInfo/latticeinfo.h \
    ForceModifier/forcemodifier.h
