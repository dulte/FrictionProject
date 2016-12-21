#include "constantforce.h"



ConstantForce::ConstantForce(vec3 force):
    m_force(force)
{

}

vec3 ConstantForce::getForceModification(){
    return m_force;
}
