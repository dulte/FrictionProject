#include "straightenerforce.h"
#include "memory"

StraightenerForce::StraightenerForce(std::vector<std::shared_ptr<Node> > nodes,std::shared_ptr<Parameters>  spParameters)
    :m_nodes(nodes),
      m_k(spParameters->m_straightener_k),
      m_nx(spParameters->m_nx)

{
}

vec3 StraightenerForce::getForceModification(){
    double sum_y = 0;
    double mean = 0;

    for(auto & node: m_nodes){
        sum_y += node->r().y();
    }
    mean = sum_y/float(m_nx);
    return vec3(0,(mean - m_node->r().y())*m_k,0);
}
