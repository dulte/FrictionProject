#include "nodeinfo.h"
#include "Node/node.h"

NodeInfo::NodeInfo(std::shared_ptr<Node> node, double d0, double phiOffset)
{
    m_node = node;
    m_d0 = d0;
    m_phiOffset = phiOffset;
}
