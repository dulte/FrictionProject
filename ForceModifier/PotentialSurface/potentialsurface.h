#pragma once
#include "Vec3/vec3.h"
#include "ForceModifier/forcemodifier.h"




class PotentialSurface : public ForceModifier
{
public:
    PotentialSurface(double k);
    vec3 getForceModification() override;
protected:
    double m_k = 0;
};
