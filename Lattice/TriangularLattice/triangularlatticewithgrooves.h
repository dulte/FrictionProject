#ifndef TRIANGULARLATTICEWITHGROOVES_H
#define TRIANGULARLATTICEWITHGROOVES_H

#include <vector>
#include "Lattice/lattice.h"
#include "DataOutput/dumpable.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"

class LatticeInfo;


class TriangularLatticeWithGrooves : public Lattice
{
public:
    TriangularLatticeWithGrooves();

    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) override;
    void populateWithUnitCell(double d, double E, double nu, double hZ, double density);
    void populateCantilever(double d, double E, double nu, double hZ, double density);
    void setNumberOfGrooves(int ng);

    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;


    int m_numberOfGrooves = 0;
    int m_nx;
    int m_ny;
};

#endif // TRIANGULARLATTICEWITHGROOVES_H
