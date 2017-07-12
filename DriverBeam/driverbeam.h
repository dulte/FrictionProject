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
class vec3;

class DriverBeam : public SimpleLattice
{
public :
    DriverBeam(std::shared_ptr<Parameters>);
    virtual ~DriverBeam();

    void attachToLattice(std::shared_ptr<Lattice>);
    void step(double dt) override;
    std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    void startDriving(){m_velocity = m_vD;};
    void stealTopNodes(std::shared_ptr<Lattice>);
    void checkRotation(int i); //TODO: This is a stupid solution

    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_topNodes;

protected :
    // Read from parameters
    const unsigned int m_nx;
    const unsigned int m_ny;
    const double       m_k;
    const double       m_vD;              // Velocity of the beam
    const double       m_mass;
    const double       m_momentOfInertia;
    const double       m_angle;           // Wanted angle
    const int          m_rotTime;         // Time it takes for the beam to go from 0 to angle
    // Not read from parameters
    double             m_phiStep;         // Change in angle required to reach angle after rotTime
    double             m_omega;           // Angular velocity
    double             m_phi;             // Current angle
    double             m_torque;          // Torque
    vec3               m_force;
    double             m_velocity;        // Current velocity of the driver in the x-direction
    vec3               m_v;
    vec3               m_center;          // Current center of the beam
    std::vector<double> m_distFromCenter;

    // Not read from parameters
    const int offset = 1;

    std::shared_ptr<Parameters> m_parameters;
};
