#pragma once

#include <memory>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "InputManagement/Parameters/parameters.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"
#include "Lattice/lattice.h"

#define pi 3.14159265358979323

class SpringFriction;
class Node;
class LatticeInfo;
class Lattice;

class DriverBeam
{
public :
    explicit DriverBeam(std::shared_ptr<Parameters> spParameters,
                        std::shared_ptr<Lattice>    spLattice);
    ~DriverBeam();

    void step(double dt);
    void construct(std::shared_ptr<Parameters> spParameters);

protected :
    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_latticeNodes;
    // Nodes connecting the driver nodes to the top nodes of the lattice.
    std::vector<std::shared_ptr<Node> > m_attachmentNodes;
    // The driver nodes pushing/pulling/driving the entire system
    std::vector<std::shared_ptr<Node> > m_driverNodes;

    // Read from spParameters
    const unsigned int m_nx;
    const unsigned int m_ny;
    const double       m_driverSprings_k;
    const double       m_attachmentSprings_k;
    const double       m_driverForce;

    // Not read from spParameters
    const int offset = 1;
    std::shared_ptr<Lattice> m_lattice;
};
