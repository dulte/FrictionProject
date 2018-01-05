#ifndef SIMPLELATTICE_H
#define SIMPLELATTICE_H

#include <memory>
#include <vector>

#include "Node/node.h"
#include "DataOutput/datapacket.h"
#include "Lattice/simplelattice.h"

class Node;
class LatticeInfo;
class Lattice;

class SimpleLattice: virtual public std::enable_shared_from_this<SimpleLattice>
{
public:
    SimpleLattice();
    virtual ~SimpleLattice();

    std::vector<std::shared_ptr<Node>> nodes;
    std::shared_ptr<LatticeInfo>       latticeInfo;

    double  t() {return m_t;}
    virtual void step(double dt) = 0;
    virtual std::vector<DataPacket> getDataPackets(int, double) = 0;
protected:
    double m_t = 0; // Simulation time
};

#endif /* SIMPLELATTICE_H */
