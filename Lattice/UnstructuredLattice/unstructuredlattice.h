#pragma once

#include <vector>
#include <memory>
#include "Lattice/lattice.h"
#include "DataOutput/dumpable.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"

class LatticeInfo;
class LatticeScanner;
class Parameters;

class UnstructuredLattice: public Lattice
{
public:
    explicit UnstructuredLattice(){};
    virtual ~UnstructuredLattice(){};

    void populate(std::shared_ptr<Parameters> parameters) override;
    virtual std::vector<DataPacket> getDataPackets(int timetep, double time) override;
protected:
    void connectNodes();
    int m_nx;
    int m_ny;
    double m_d;
};
