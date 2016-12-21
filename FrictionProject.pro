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
    ForceModifier/forcemodifier.cpp \
    ForceModifier/PotentialSurface/potentialsurface.cpp \
    ForceModifier/RelativeVelocityDamper/relativevelocitydamper.cpp \
    ForceModifier/PotentialPusher/potentialpusher.cpp \
    ForceModifier/ConstantMoment/constantmoment.cpp \
    ForceModifier/ConstantForce/constantforce.cpp \
    ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.cpp \
    ForceModifier/SpringFriction/springfriction.cpp \
    FrictionInfo/frictioninfo.cpp


HEADERS += \
    Lattice/lattice.h \
    Node/node.h \
    NodeInfo/nodeinfo.h \
    Vec3/vec3.h \
    LatticeInfo/latticeinfo.h \
    ForceModifier/forcemodifier.h \
    ForceModifier/PotentialSurface/potentialsurface.h \
    ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h \
    ForceModifier/PotentialPusher/potentialpusher.h \
    ForceModifier/ConstantMoment/constantmoment.h \
    ForceModifier/ConstantForce/constantforce.h \
    ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h \
    ForceModifier/SpringFriction/springfriction.h \
    FrictionInfo/frictioninfo.h

