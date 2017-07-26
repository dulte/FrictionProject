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

#define pi 3.14159265358979323

DriverBeam::DriverBeam(std::shared_ptr<Parameters>  parameters,
                       std::shared_ptr<Lattice>     lattice):
    m_lattice(lattice),
    m_nx(parameters->m_nx),
    m_ny(parameters->m_ny),
    m_vD(parameters->m_vD),
    m_angle(parameters->m_beamAngle/180*pi), // convert from degrees to radians
    m_rotTime(parameters->m_beamRotTime),
    m_phiStep(m_angle/m_rotTime),
    m_parameters(parameters),
    Node(vec3(), parameters->m_beamMass, parameters->m_beamMass*(parameters->m_d*m_nx)*(parameters->m_d*m_nx)/12.0, lattice->latticeInfo)
{
}

DriverBeam::~DriverBeam(){}

void DriverBeam::attachToLattice(){
    // steal the top nodes from the lattice so as to not allow lattice to
    // integrate them
    stealTopNodes(m_lattice);

    // find the center of the beam
    for (auto & node : m_nodes)
        m_r += node->r();
    m_r /= m_nodes.size();

    // store the default x-distance from the center for each node
    for (size_t i = 0; i < m_nodes.size(); i++){
        m_distFromCenter.push_back(m_r[0]-m_nodes[i]->r()[0]);
    }

}

void DriverBeam::stealTopNodes(std::shared_ptr<Lattice> lattice){
    // this is quite inefficient, but as it is only called once, it doesn't matter

    // add lattice's topnodes to the beam
    for(auto & topnode : lattice->topNodes){
        // remove the topNodes from the nodes and leftNodes
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
        m_nodes.push_back(topnode);
    }
    // then, clear the topNodes (is this necessary??) YES!
    lattice->topNodes.clear();
    // Distribute the mass of the driver beam onto the nodes
    for (auto& node: m_nodes)
        node->setMass(m_mass/m_nodes.size());
}

void DriverBeam::updateForcesAndMoments(){
    m_moment = 0;
    m_f      = 0;
#pragma omp parallel for
    for (size_t i = 0; i < m_nodes.size(); i++){
        m_nodes[i]->updateForcesAndMoments();
        m_moment += m_nodes[i]->f().cross2d(m_r);
        m_f += m_nodes[i]->f();
    }

}

void DriverBeam::vvstep(double dt){
    m_phi += m_phiStep;
    m_v   += (m_f/m_mass)*0.5*dt;
    m_v[0] = m_velocity;
    m_r   += m_v*dt;

#pragma omp parallel for
    for (size_t i = 0; i < m_nodes.size(); i++){
        vec3 r = m_r;
        r[0] -= cos(m_phi)*m_distFromCenter[i];
        r[1] += sin(m_phi)*m_distFromCenter[i];
        m_nodes[i]->forcePosition(r);
    }
}

std::vector<DataPacket> DriverBeam::getDataPackets(int timestep, double time){
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket torque   = DataPacket(DataPacket::dataId::BEAM_TORQUE, timestep, time);
    DataPacket shearForce = DataPacket(DataPacket::dataId::BEAM_SHEAR_FORCE, timestep, time);

    for (std::shared_ptr<Node> node : m_nodes) {
        torque.push_back(node->moment());
        shearForce.push_back(node->f().x());
    }
    packetvec.push_back(torque);
    packetvec.push_back(shearForce);
    return packetvec;
}

void DriverBeam::checkRotation(){
    if (m_phi >= m_angle){
        m_phiStep = 0;
        m_phi = m_angle;
    }
}
