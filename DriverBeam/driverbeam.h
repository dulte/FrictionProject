#pragma once

#include <memory>
#include <vector>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "Lattice/simplelattice.h"
#define pi 3.14159265358979323

class SpringFriction;
class Lattice;
class Node;
class Parameters;
class PotentialPusher;

class DriverBeam : public SimpleLattice
{
public :
    DriverBeam(std::shared_ptr<Parameters>);
    virtual ~DriverBeam();

    void attachToLattice(std::shared_ptr<Lattice>);
    void step(double dt) override;
    std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    std::vector<std::shared_ptr<PotentialPusher>> addPushers(double tInit);
    void stealTopNodes(std::shared_ptr<Lattice>);

    // All nodes
    // std::vector<std::shared_ptr<Node> > m_nodes;
    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_topNodes;
    // The driver nodes pushing/pulling/driving the entire system
    std::vector<std::shared_ptr<Node> > m_driverNodes;

protected :

    // Read from parameters
    const unsigned int m_nx;
    const unsigned int m_ny;
    const double       m_driverSprings_k;
    const double       m_driverVD;

    // Not read from parameters
    const int offset = 1;

    std::shared_ptr<Parameters> m_parameters;
};
