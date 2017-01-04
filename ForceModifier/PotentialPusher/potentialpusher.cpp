#include "potentialpusher.h"
#include "Node/node.h"
#include <iostream>

PotentialPusher::PotentialPusher(double k, double vD, double xInit, double tInit)
      :
//    m_k(k),
//    m_vD(vD),
      m_xInit(xInit),
      m_tInit(tInit)
{
    m_k = input->get("pK");
    m_vD = input->get("vD");
    //m_xInit = input->get("xInit");
    //m_tInit = input->get("tInit");
}

vec3 PotentialPusher::getForceModification(){
    fPush = m_k*(m_xInit + m_vD*(m_node->t() - m_tInit) - m_node->r()[0]);
//    std::cout << m_node->t() << endl;
    return vec3(fPush,0,0);
}
