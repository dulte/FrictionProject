#pragma once

#include "ForceModifier/forcemodifier.h"
#include "Vec3/vec3.h"

class PotentialPusher: public ForceModifier
{
public:
    PotentialPusher(double k, double vD, double xInit, double tInit);
    vec3 getForceModification() override;
    double fPush;

protected:
    double m_k = 0;
    double m_vD = 0;
    double m_xInit = 0;
    double m_tInit = 0;
};

