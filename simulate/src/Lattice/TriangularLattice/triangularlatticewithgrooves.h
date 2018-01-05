#ifndef TRIANGULARLATTICEWITHGROOVES_H
#define TRIANGULARLATTICEWITHGROOVES_H

#include <vector>
#include "Lattice/lattice.h"
#include "DataOutput/dumpable.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"
#include "DriverBeam/driverbeam.h"

class LatticeInfo;

class TriangularLatticeWithGrooves : public Lattice
{
public:
    TriangularLatticeWithGrooves();

    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density, int grooveSize, int grooveHeight);
    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density);
    void populateSymmetric(int nx , int ny, double d, double E, double nu, double hZ, double density, int grooveSize, int grooveHeight);
    void populateWithUnitCell(double d, double E, double nu, double hZ, double density);
    void populateCantilever(double d, double E, double nu, double hZ, double density);

    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
 private:
    int m_grooveSize;
    int m_grooveHeight;
    int m_nx;
    int m_ny;

};

#endif // TRIANGULARLATTICEWITHGROOVES_H
