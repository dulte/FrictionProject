#pragma once
#include <memory>
#include <vector>

#include "Node/node.h"

class Node;
class LatticeInfo;
class Parameters;
class DataPacket;

class Lattice : virtual public std::enable_shared_from_this<Lattice>
{
public:
    Lattice();
    ~Lattice();
    std::vector<std::shared_ptr<Node>> bottomNodes;
    std::vector<std::shared_ptr<Node>> topNodes;
    std::vector<std::shared_ptr<Node>> leftNodes;
    std::vector<std::shared_ptr<Node>> normalNodes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::shared_ptr<LatticeInfo>       latticeInfo;
    double  t() {return m_t;}
    std::string xyzRepresentation();

    virtual void step(double dt);
    virtual void populate(std::shared_ptr<Parameters> parameters) = 0;
    virtual void populate(std::shared_ptr<Parameters>, int nx, int ny) = 0;
    static std::shared_ptr<LatticeInfo> latticeInfoFromParameters(std::shared_ptr<Parameters> parameters);
    static std::shared_ptr<Node> newNode(std::shared_ptr<Parameters>, std::shared_ptr<LatticeInfo>,
                                         double x, double y);
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time);
protected:
    double m_t = 0; // Simulation time
};

