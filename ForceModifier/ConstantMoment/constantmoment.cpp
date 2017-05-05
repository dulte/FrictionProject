#include "constantmoment.h"


ConstantMoment::ConstantMoment(double moment):
    m_moment(moment)
{
}

double ConstantMoment::getMomentModification(){
    return m_moment;
}
