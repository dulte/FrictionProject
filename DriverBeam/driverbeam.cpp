#include "driverbeam.h"

DriverBeam::DriverBeam(std::shared_ptr<Parameters>  spParameters,
                       std::shared_ptr<Lattice>     spLattice):
    m_nx(spParameters->m_nx),
    m_ny(spParameters->m_ny),
    m_driverSprings_k(spParameters->m_driverSprings_k),
    m_attachmentSprings_k(spParameters->m_attachmentSprings_k),
    m_driverForce(spParameters->m_driverForce),
    m_lattice(spLattice),
    m_latticeNodes(spLattice->topNodes)
{
    construct(spParameters);
}

DriverBeam::~DriverBeam(){}

void DriverBeam::construct(std::shared_ptr<Parameters> spParameters){
    const double d       = spParameters->m_d;
    const double hZ      = spParameters->m_hZ;
    const double density = spParameters->m_density;

    // Construct the attachment nodes
    double ry = d*(m_ny+1);
    for (size_t i = 0; i < m_nx; i++) {
        double rx = i*d;
        vec3 pos(rx, ry, 0);

        std::shared_ptr<Node> node = std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8,
                                                            m_lattice->latticeInfo);
        m_attachmentNodes.push_back(node);
    }

    // Connect the attachment to the top nodes of the lattice
    for (size_t i = 0; i < m_nx; i++) {
        m_attachmentNodes[i]->setLattice(m_lattice);
        m_attachmentNodes[i]->connectToNode(m_latticeNodes[i]);
    }
}
