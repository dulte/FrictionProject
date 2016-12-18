#pragma once
#include <memory>

class Node;


class NodeInfo
{
public:
    NodeInfo(std::shared_ptr<Node> node, double d0, double phiOffset);
    double d0(){return m_d0;}
    double phiOffset(){return m_phiOffset;}
    std::shared_ptr<Node> node(){return m_node;}
private:
    std::shared_ptr<Node> m_node;
    double m_phiOffset;
    double m_d0;
};

