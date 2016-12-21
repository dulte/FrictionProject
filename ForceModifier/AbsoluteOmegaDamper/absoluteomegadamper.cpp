#include "absoluteomegadamper.h"
#include "Node/node.h"

AbsoluteOmegaDamper::AbsoluteOmegaDamper(double eta):
    m_eta(eta)
{

}

double AbsoluteOmegaDamper::getMomentModification(){
    return -m_eta*m_node->omega();
}
