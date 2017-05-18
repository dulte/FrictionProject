#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory.h>
#include <vector>
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/TriangularLattice/triangularlatticewithgrooves.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "DriverBeam/driverbeam.h"
#include <fstream>
#include <string>

class SpringFriction;
class PotentialPusher;

class SidePotentialLoading : public Dumpable
{
public:
    SidePotentialLoading(std::shared_ptr<Parameters> spParameters);
    ~SidePotentialLoading();
    std::shared_ptr<TriangularLatticeWithGrooves> lattice;
    // Add pusher force to the driver beam
    void addDriverForce(double);
    // Add the pusher nodes as described by pusherStartHeight and pusherEndHeight
    void addPusher(double tInit);
    // Un/lock springs
    void isLockFrictionSprings(bool);
    // Call the lattice and driverbeam's step
    void step(double);
    /* /void dumpParameters(); */
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    virtual std::string xyzString() const;

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    std::shared_ptr<DriverBeam> m_driverBeam;

    std::ofstream outfileParameters;
    std::string outFileFolder = "";
protected:
    int    m_pusherStartHeight;
    int    m_pusherEndHeight;
    double m_vD;
    double m_k;
};

#endif // SIDEPOTENTIALLOADING_H
