#ifndef TRIANGULARLATTICE_H
#define TRIANGULARLATTICE_H

#include <vector>
#include "Lattice/lattice.h"
#include "DataOutput/dumpable.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"

class LatticeInfo;

class TriangularLattice : public Lattice
{
public:
    TriangularLattice();

    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) override;
    void populateWithUnitCell(double d, double E, double nu, double hZ, double density);
    void populateCantilever(double d, double E, double nu, double hZ, double density);

    virtual std::vector<DataPacket> getDataPackets(int timetep, double time) override;
 private:
    int m_nx;
    int m_ny;
};

#endif // TRIANGULARLATTICE_H
