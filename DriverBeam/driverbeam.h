#pragma once

#include <memory>
#include <vector>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "Lattice/simplelattice.h"
#include "DataOutput/datapacket.h"
#define pi 3.14159265358979323

class SpringFriction;
class Lattice;
class Node;
class Parameters;
class PotentialPusher;
class vec3;

class DriverBeam : public Node
{
public :
    DriverBeam(std::shared_ptr<Parameters>,
               std::shared_ptr<Lattice>);
    virtual ~DriverBeam();

    void attachToLattice();
    std::vector<DataPacket> getDataPackets(int timestep, double time);
    void startDriving(){m_velocity = m_vD; m_isDriving=true;};
    void stealTopNodes(std::shared_ptr<Lattice>);
    void updateForcesAndMoments();
    void vvstep(double dt);
    void correctVelocity();
    void beginCorrectVelocity();
    double totalShearForce();

    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_nodes;
    std::shared_ptr<Lattice> m_lattice;

protected :
    // Read from parameters
    int    m_nx;
    int    m_ny;
    double m_vD;              // Velocity of the beam
    double m_angle;           // Wanted angle
    int    m_rotTime;         // Time it takes for the beam to go from 0 to angle
    int    m_velocityTime;    // Time it takes for the beam to go to the desired velocity
    double m_beamMass;
    // Not read from parameters
    double m_phiStep;         // Change in angle required to reach angle after rotTime
    double m_velocity;        // Current velocity of the driver in the x-direction
    double m_velocityStep;    // Change in velocity required to reach the desired velocity after velocityTime
    double m_initalVel;       // The inital velocity from which velocityStep begins
    std::vector<double> m_distFromCenter;
    std::shared_ptr<Parameters> m_parameters;
    bool m_isDriving = false;
};
