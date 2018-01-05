#pragma once
#include "ForceModifier/forcemodifier.h"



class RelativeVelocityDamper: public ForceModifier
{
public:
    RelativeVelocityDamper(double eta);
    vec3 getForceModification() override;
protected:
    double m_eta = 0;
};

