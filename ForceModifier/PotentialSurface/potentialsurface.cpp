#include "potentialsurface.h"
#include "Node/node.h"

PotentialSurface::PotentialSurface(double k)
{
    m_k = k;
}

vec3 PotentialSurface::getForceModification(){

    vec3 r = m_node->r();
    if (r[1] < 0){
        return vec3(0,-m_k*r[1],0);
    }
    else{
        return vec3(0,0,0);
    }
}
