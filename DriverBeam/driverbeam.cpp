#include <memory>
#include <omp.h>
#include <iostream>
#include <cmath>
#include "driverbeam.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/lattice.h"
#include "ForceModifier/PotentialPusher/potentialpusher.h"

DriverBeam::DriverBeam(std::shared_ptr<Parameters>  parameters):
    m_nx(parameters->m_nx),
    m_ny(parameters->m_ny),
    m_driverSprings_k(parameters->m_driverSprings_k),
    m_driverVD(parameters->m_driverVD),
    m_parameters(parameters)
{
}

DriverBeam::~DriverBeam(){}

void DriverBeam::attachToLattice(std::shared_ptr<Lattice> lattice){
    const double d       = m_parameters->m_d;

    // Steal the top nodes from the Lattice so as to no allow lattice to
    // integrate them
    stealTopNodes(lattice);
    // Construct the driver nodes
    double ry = d*(1+m_ny)*sin(pi/3);
    for (size_t i = 0; i < m_nx; i++){
        double rx = i*d+((m_ny+1)%2)*d*cos(pi/3);
        std::shared_ptr<Node> node = Lattice::newNode(m_parameters, lattice->latticeInfo,
                                                      rx, ry);
        m_driverNodes.push_back(node);
        nodes.push_back(node);
    }

}

void DriverBeam::stealTopNodes(std::shared_ptr<Lattice> lattice){
    // This is quite inefficient, but as it is only called once, it doesn't matter
    // Add lattice's topnodes to the beam
    for(auto & topnode : lattice->topNodes){
        // Remove the topnodes from the nodes and leftnodes
        for(auto it = lattice->leftNodes.begin(); it != lattice->leftNodes.end();){
            if(topnode == *it)
                it = lattice->leftNodes.erase(it);
            else
                ++it;
        }
        for(auto it = lattice->nodes.begin(); it != lattice->nodes.end();){
            if(topnode == *it)
                it = lattice->nodes.erase(it);
            else
                ++it;
        }
        m_topNodes.push_back(topnode);
        nodes.push_back(topnode);
    }
    // Then, clear the topnodes (is this necessary??)
    lattice->topNodes.clear();

    // Connect the driver nodes to the top nodes
    double d = m_parameters->m_d;
    for (auto & node : nodes){
        // TODO: Make shared_from_this from DriverBeam
        // WHY DOESN'T THAT WORK!? AAAARG!
        node->setLattice(shared_from_this());
        for (auto & node2 : nodes){
            if (node->distanceTo(*node2) < d*1.01 && node->distanceTo(node2) > d*0.01)
                node->connectToNode(node2);
        }
    }
}

std::vector<std::shared_ptr<PotentialPusher>> DriverBeam::addPushers(double tInit){
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    for (auto & node : m_driverNodes){
        std::shared_ptr<PotentialPusher> pusher = std::make_shared<PotentialPusher>(m_driverSprings_k, m_driverVD, node->r().x(), tInit);
        pusherNodes.push_back(pusher);
        node->addModifier(std::move(pusher)); // TODO: Why move here?
    }
    return pusherNodes;
}

void DriverBeam::step(double dt){
    omp_set_num_threads(4);
#pragma omp flush(dt)

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep1(dt);
    }

    m_t += dt*0.5;

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {

        nodes[i]->updateForcesAndMoments();
    }

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep2(dt);
    }
    m_t += dt*0.5;
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
