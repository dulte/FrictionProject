#include "potentialpusher.h"
#include "Node/node.h"

PotentialPusher::PotentialPusher(double k, double vD, double xInit, double tInit)
      :
    m_k(k),
    m_vD(vD),
    m_xInit(xInit),
    m_tInit(tInit)
{
}

vec3 PotentialPusher::getForceModification(){
    fPush = m_k*(m_xInit + m_vD*(m_node->t() - m_tInit) - m_node->r().x());
    return vec3(fPush,0,0);
}
