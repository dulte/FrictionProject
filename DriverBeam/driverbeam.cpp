#include <memory>
#include <iostream>
#include <cmath>
#include "driverbeam.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/lattice.h"
#include "ForceModifier/PotentialPusher/potentialpusher.h"

DriverBeam::DriverBeam(std::shared_ptr<Parameters>  spParameters,
                       std::shared_ptr<Lattice>     spLattice):
    m_nx(spParameters->m_nx),
    m_ny(spParameters->m_ny),
    m_driverSprings_k(spParameters->m_driverSprings_k),
    m_attachmentSprings_k(spParameters->m_attachmentSprings_k),
    m_driverForce(spParameters->m_driverForce),
    m_driverVD(spParameters->m_driverVD),
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
    // The y-coordinate is one above the lattice
    double ry = d*m_ny*sin(pi/3);
    for (size_t i = 0; i < m_nx; i++) {
        double rx = i*d+(m_ny%2)*d*cos(pi/3);
        vec3 pos(rx, ry, 0);

        std::shared_ptr<Node> node = std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8,
                                                            m_lattice->latticeInfo);
        m_attachmentNodes.push_back(node);
        nodes.push_back(node);
    }
    // Construct the driver nodes
    ry = d*(1+m_ny)*sin(pi/3);
    for (size_t i = 0; i < m_nx; i++){
        double rx = i*d+((m_ny+1)%2)*d*cos(pi/3);
        vec3 pos(rx, ry, 0);

        std::shared_ptr<Node> node = std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8,
                                                            m_lattice->latticeInfo);
        m_driverNodes.push_back(node);
        nodes.push_back(node);
    }

    // Connect the attachment to the top nodes of the lattice
    // and the driver nodes to the attachment nodes
    for (size_t i = 0; i < m_nx; i++) {
        // TODO: Make shared_from_this from DriverBeam
        m_attachmentNodes[i]->setLattice(m_lattice->shared_from_this());
        m_attachmentNodes[i]->connectToNode(m_latticeNodes[i]);
        m_driverNodes[i]->setLattice(m_lattice->shared_from_this());
        m_driverNodes[i]->connectToNode(m_attachmentNodes[i]);
    }
}

std::vector<std::shared_ptr<PotentialPusher>> DriverBeam::addDriverForce(double tInit){
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    for (auto & node : m_driverNodes){
        std::shared_ptr<PotentialPusher> pusher = std::make_shared<PotentialPusher>(m_driverSprings_k, m_driverVD, node->r().x(), tInit);
        pusherNodes.push_back(pusher);
        node->addModifier(std::move(pusher));
    }
    return pusherNodes;
}

std::vector<DataPacket> DriverBeam::getDataPackets(int timestep, double time){
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket position = DataPacket(DataPacket::dataId::BEAM_POSITION, timestep, time);
    DataPacket velocity = DataPacket(DataPacket::dataId::BEAM_VELOCITY, timestep, time);

    for (std::shared_ptr<Node> node : nodes) {
        position.push_back(node->r().x());
        position.push_back(node->r().y());
        velocity.push_back(node->v().x());
        velocity.push_back(node->v().y());
    }
    packetvec.push_back(position);
    packetvec.push_back(velocity);
    return packetvec;
}
