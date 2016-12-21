#pragma once
#include "Vec3/vec3.h"
#include "ForceModifier/forcemodifier.h"


class ConstantForce: public ForceModifier
{
public:
    ConstantForce(vec3 force);

    vec3 getForceModification() override;
protected:
    vec3 m_force;
};
