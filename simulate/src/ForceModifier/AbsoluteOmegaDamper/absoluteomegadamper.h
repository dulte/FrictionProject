#pragma once

#include "ForceModifier/forcemodifier.h"


class AbsoluteOmegaDamper: public ForceModifier
{
public:
    AbsoluteOmegaDamper(double eta);
    double getMomentModification() override;
protected:
    double m_eta = 0;
};

