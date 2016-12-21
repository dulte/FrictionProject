#include "relativevelocitydamper.h"
#include "Node/node.h"
#include "NodeInfo/nodeinfo.h"

RelativeVelocityDamper::RelativeVelocityDamper(double eta)
{
    m_eta = eta;
}

vec3 RelativeVelocityDamper::getForceModification(){
    vec3 m_dampforce(0,0,0);

    for (auto & neighbour: m_node->getNeighborInfo()){
        m_dampforce += m_eta*(neighbour->node()->v() - m_node->v());
    }

    return m_dampforce;

}
