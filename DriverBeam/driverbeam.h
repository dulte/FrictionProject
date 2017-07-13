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

class DriverBeam : public Node
{
public :
    DriverBeam(std::shared_ptr<Parameters>,
               std::shared_ptr<Lattice>);
    virtual ~DriverBeam();

    void attachToLattice();
    std::vector<DataPacket> getDataPackets(int timestep, double time);
    void startDriving(){m_velocity = m_vD;};
    void stealTopNodes(std::shared_ptr<Lattice>);
    void checkRotation(); //TODO: This is a stupid solution
    void updateForcesAndMoments();
    void vvstep(double dt);

    // The top nodes of to lattice to which the attachment nodes are attached
    std::vector<std::shared_ptr<Node> > m_nodes;
    std::shared_ptr<Lattice> m_lattice;

protected :
    // Read from parameters
    const unsigned int m_nx;
    const unsigned int m_ny;
    const double       m_vD;              // Velocity of the beam
    const double       m_angle;           // Wanted angle
    const int          m_rotTime;         // Time it takes for the beam to go from 0 to angle
    // Not read from parameters
    double             m_phiStep;         // Change in angle required to reach angle after rotTime
    double             m_velocity;        // Current velocity of the driver in the x-direction
    std::vector<double> m_distFromCenter;

    std::shared_ptr<Parameters> m_parameters;
};
