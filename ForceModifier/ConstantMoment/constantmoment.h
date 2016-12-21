#pragma once

#include "ForceModifier/forcemodifier.h"


class ConstantMoment: public ForceModifier
{
public:
    ConstantMoment(double moment);
    double getMomentModification() override;
protected:
    double m_moment = 0;
};

