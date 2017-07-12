#include <memory>
#include <omp.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <math.h>
#include "driverbeam.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/lattice.h"
#include "ForceModifier/PotentialPusher/potentialpusher.h"
#include "Vec3/vec3.h"

#define PI 3.14159265358979323

DriverBeam::DriverBeam(std::shared_ptr<Parameters>  parameters):
    m_nx(parameters->m_nx),
    m_ny(parameters->m_ny),
    m_k(parameters->m_k),
    m_vD(parameters->m_vD),
    m_mass(parameters->m_beamMass),
    m_momentOfInertia(m_mass*(parameters->m_d*m_nx)*(parameters->m_d*m_nx)/12.0), // Ml^2/12
    m_angle(parameters->m_beamAngle/180*PI), // Convert from degrees to radians
    m_rotTime(parameters->m_beamRotTime),
    m_phiStep(m_angle/m_rotTime),
    m_omega(0),
    m_phi(0),
    m_torque(0),
    m_force(vec3()),
    m_v(vec3()),
    m_center(vec3()),
    m_parameters(parameters)
{
}

DriverBeam::~DriverBeam(){}

void DriverBeam::attachToLattice(std::shared_ptr<Lattice> lattice){
    // Steal the top nodes from the Lattice so as to no allow lattice to
    // integrate them
    stealTopNodes(lattice);

    // Set this as the lattice
    for (auto & node : nodes)
        node->setLattice(shared_from_this());
    // Find the center of the beam
    for (auto & node : m_topNodes)
        m_center += node->r();
    m_center /= m_topNodes.size();

    // Store the default x-distance from the center for each node
    for (size_t i = 0; i < m_topNodes.size(); i++){
        m_distFromCenter.push_back(m_center[0]-m_topNodes[i]->r()[0]);
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
}

/*
std::vector<std::shared_ptr<PotentialPusher>> DriverBeam::addPushers(double tInit){
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    for (auto & node : m_topNodes){
        std::shared_ptr<PotentialPusher> pusher = std::make_shared<PotentialPusher>(m_k, m_vD, node->r().x(), tInit);
        pusherNodes.push_back(pusher);
        node->addModifier(std::move(pusher)); // TODO: Why move here?
    }
    return pusherNodes;
}
*/

void DriverBeam::step(double dt){
    omp_set_num_threads(4);
#pragma omp flush(dt)

#pragma omp parallel for
    for (size_t i = 0; i < nodes.size(); i++){
        nodes[i]->updateForcesAndMoments();
    }
    m_torque = 0;
    m_force = 0;
#pragma omp parallel for
    for (size_t i = 0; i < nodes.size(); i++){
        // m_torque += nodes[i]->f().cross2d(m_center);
        m_force += nodes[i]->f();
    }
    // m_omega  += (m_torque/m_momentOfInertia)*dt;
    // m_phi    += m_omega*dt;
    m_phi    += m_phiStep;
    m_v      += (m_force/m_mass)*dt;
    m_v[0]   = m_velocity;
    m_center += m_v*dt;

#pragma omp parallel for
    for (size_t i = 0; i < nodes.size(); i++){
        vec3 r = m_center;
        r[0] -= cos(m_phi)*m_distFromCenter[i];
        r[1] += sin(m_phi)*m_distFromCenter[i];
        nodes[i]->forcePosition(r);
    }
    m_t += dt;
// #pragma omp parallel for
// //     for (size_t i = 0; i<nodes.size(); i++)
//     {
//         nodes[i]->vvstep1(dt);
//     }

//     m_t += dt*0.5;

// #pragma omp parallel for
//     for (size_t i = 0; i<nodes.size(); i++)
//     {

//         nodes[i]->updateForcesAndMoments();
//     }

// #pragma omp parallel for
//     for (size_t i = 0; i<nodes.size(); i++)
//     {
//         nodes[i]->vvstep2(dt);
//     }
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

void DriverBeam::checkRotation(int i){
    if (i > m_rotTime)
        m_phiStep = 0;
}
