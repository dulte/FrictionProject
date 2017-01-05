#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory>
#include <vector>
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/TriangularLattice/triangularlatticewithgrooves.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include "InputManagement/ConfigReader/configreader.h"
#include <fstream>

class SpringFriction;
class PotentialPusher;

class SidePotentialLoading : public Dumpable
{
public:
    SidePotentialLoading(int nx, int ny, double d, double E, double k, double topLoadingForce);
    ~SidePotentialLoading();
//    std::shared_ptr<TriangularLattice> lattice;
    std::shared_ptr<TriangularLatticeWithGrooves> lattice;
    void addPusher(double k, double vD, double tInit);
    void isLockFrictionSprings(bool);
    void dumpData();
    void dumpParameters();
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;

    ConfigReader *input = new ConfigReader("config.txt");

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    std::ofstream outfile;
    std::ofstream outfileNormalForces;
    std::ofstream outfilePusherForces;
    std::ofstream outfileParameters;
};

#endif // SIDEPOTENTIALLOADING_H
