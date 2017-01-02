#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Lattice/lattice.h"
#include "Node/node.h"
#include "DataOutput/datapacket.h"

class Node;
class LatticeInfo;


class Lattice : public std::enable_shared_from_this<Lattice>//, public Dumpable
{
public:
    Lattice();
    ~Lattice();
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Node>> bottomNodes;
    std::vector<std::shared_ptr<Node>> topNodes;
    std::vector<std::shared_ptr<Node>> leftNodes;
    std::shared_ptr<LatticeInfo> latticeInfo;

    double t() {return m_t;}

    virtual void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) = 0;
    virtual void step(double dt);
    virtual std::string xyzString();
    virtual std::vector<DataPacket> getDataPackets(int, double) = 0;

protected:
    double m_t=0; // Simulation time

};

