#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory>
#include <vector>
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/TriangularLattice/triangularlatticewithgrooves.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include "InputManagement/Parameters/parameters.h"
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
    void addPusher(double tInit);
    void isLockFrictionSprings(bool);
    /* /void dumpParameters(); */
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;

    std::ofstream outfileParameters;
    std::string outFileFolder = "";
private:
    int m_pusherStartHeight;
    int m_pusherEndHeight;
    double m_vD;
    double m_k;
};

#endif // SIDEPOTENTIALLOADING_H
