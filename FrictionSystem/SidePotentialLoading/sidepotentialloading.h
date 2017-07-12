#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory.h>
#include <vector>
#include <fstream>
#include <string>
#include "DriverBeam/driverbeam.h"
#include "Lattice/UnstructuredLattice/unstructuredlattice.h"

class SpringFriction;
class PotentialPusher;
class UnstructuredLattice;

class SidePotentialLoading : public Dumpable
{
public:
    SidePotentialLoading(std::shared_ptr<Parameters> parameters);
    ~SidePotentialLoading();
    // Add pusher force to the driver beam
    // void addDriverForce(double);
    // Add the pusher nodes as described by pusherStartHeight and pusherEndHeight
    void addPusher(double tInit);
    void addDriver(double tInit);
    // Un/lock springs
    void isLockFrictionSprings(bool);
    // Call the lattice and driverbeam's step
    void step(double);
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    virtual std::string xyzString() const;

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    std::shared_ptr<Lattice> lattice;
    std::shared_ptr<DriverBeam> m_driverBeam;

protected:
    int    m_pusherStartHeight;
    int    m_pusherEndHeight;
    double m_vD;
    double m_k;
};

#endif // SIDEPOTENTIALLOADING_H
