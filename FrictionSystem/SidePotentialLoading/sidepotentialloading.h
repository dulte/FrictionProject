#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory>
#include <vector>
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include <fstream>

class SpringFriction;
class PotentialPusher;

class SidePotentialLoading : public Dumpable
{
public:
    SidePotentialLoading(int nx, int ny, double d, double E, double k, double topLoadingForce);
    ~SidePotentialLoading();
    std::shared_ptr<TriangularLattice> lattice;
    void addPusher(double k, double vD, double tInit);
    void isLockFrictionSprings(bool);
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
};

#endif // SIDEPOTENTIALLOADING_H
