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

class DriverBeam: public SimpleLattice
{
public :
    explicit DriverBeam(std::shared_ptr<Parameters> spParameters,
                        std::shared_ptr<Lattice>    spLattice);
    ~DriverBeam();

    void construct(std::shared_ptr<Parameters> spParameters);
    std::vector<DataPacket> getDataPackets(int, double) override;
    std::vector<std::shared_ptr<PotentialPusher>> addDriverForce(double);

    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_latticeNodes;
    // Nodes connecting the driver nodes to the top nodes of the lattice.
    std::vector<std::shared_ptr<Node> > m_attachmentNodes;
    // The driver nodes pushing/pulling/driving the entire system
    std::vector<std::shared_ptr<Node> > m_driverNodes;

protected :

    // Read from spParameters
    const unsigned int m_nx;
    const unsigned int m_ny;
    const double       m_driverSprings_k;
    const double       m_attachmentSprings_k;
    const double       m_driverForce;
    const double       m_driverVD;

    // Not read from spParameters
    const int offset = 1;
    std::shared_ptr<Lattice> m_lattice;
};
