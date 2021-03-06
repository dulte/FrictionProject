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
                       std::shared_ptr<Lattice>     lattice)
    :
    Node(vec3(), 0, 0, lattice->latticeInfo),
    m_lattice(lattice),
    m_parameters(parameters)
{
    m_nx       = parameters->get<int>("nx");
    m_ny       = parameters->get<int>("ny");
    m_vD       = parameters->get<double>("vD");
    m_angle    = parameters->get<double>("beamAngle");
    m_rotTime  = parameters->get<int>("beamRotTime");
    m_phiStep  = m_angle/m_rotTime;
    m_beamMass = parameters->get<double>("beamMass");
    m_velocityTime = parameters->get<int>("accelerationPeriod");
    m_velocity = 0;
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
        m_nodes[i]->setPhi(0.0);
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
    // lattice->topNodes.clear();
    // Sum the nodes3
    for (auto& node: m_nodes){
        m_mass += node->mass();
    }
    m_mass += m_beamMass;
    double d  = m_parameters->get<double>("d");
    m_momentOfInertia = m_mass*d*m_nx*d*m_nx/12.0;
}

void DriverBeam::updateForcesAndMoments(){
    m_moment = 0;
    m_f      = 0;
// #pragma omp parallel for
    for (size_t i = 0; i < m_nodes.size(); i++){
        m_nodes[i]->updateForcesAndMoments();
        m_moment += -m_nodes[i]->f().cross2d(m_r-m_nodes[i]->r());
        m_f      += m_nodes[i]->f();
    }

}

void DriverBeam::vvstep(double dt){
    // m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    // m_phi   += m_omega*dt;
    m_v     += (m_f/m_mass)*0.5*dt;

    if (m_isDriving){
        m_v[0] = correctVelocity();
        m_phi  = m_angle;
    }
    else
        m_phi += m_phiStep;

    m_r   += m_v*dt;

// #pragma omp parallel for
    // Align the top nodes along the axis of the beam
    for (size_t i = 0; i < m_nodes.size(); i++){
        // m_nodes[i]->m_omega += (m_nodes[i]->m_moment/m_nodes[i]->m_momentOfInertia)*0.5*dt;
        // / m_nodes[i]->m_phi += m_nodes[i]->m_omega*dt;
        m_nodes[i]->m_phi = m_phi;

        vec3 r = m_r;
        r[0] -= cos(m_phi)*m_distFromCenter[i];
        r[1] += sin(m_phi)*m_distFromCenter[i];
        m_nodes[i]->forcePosition(r);
        m_nodes[i]->forceVelocity(m_v);
    }
}

std::vector<DataPacket> DriverBeam::getDataPackets(int timestep, double time){
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket torque     = DataPacket(DataPacket::dataId::BEAM_TORQUE, timestep, time);
    DataPacket shearForce = DataPacket(DataPacket::dataId::BEAM_SHEAR_FORCE, timestep, time);

    for (std::shared_ptr<Node> node : m_nodes) {
        torque.push_back(node->moment());
        shearForce.push_back(node->f().x());
    }
    packetvec.push_back(torque);
    packetvec.push_back(shearForce);
    return packetvec;
}

double DriverBeam::correctVelocity(){
    if (m_velocityStepCounter < m_velocityTime){
            m_velocityStepCounter++;
            return m_velocityStep*m_velocityStepCounter;
    } else {
        return m_velocity;
    }
}

void DriverBeam::beginCorrectVelocity(){
    m_initalVel = m_v[0];
    m_velocityStep = (m_velocity-m_initalVel)/m_velocityTime;
}

double DriverBeam::totalShearForce(){
    double force = 0;
    for (const auto& node : m_nodes)
        force += node->f().x();
    return force;
}
