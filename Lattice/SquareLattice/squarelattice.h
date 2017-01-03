#ifndef SQUARELATTICE_H
#define SQUARELATTICE_H

#include "Lattice/lattice.h"
#include "LatticeInfo/latticeinfo.h"

class LatticeInfo;

class SquareLattice : public Lattice
{
public:
    SquareLattice();
    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) override;
        virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
};

#endif // SQUARELATTICE_H
