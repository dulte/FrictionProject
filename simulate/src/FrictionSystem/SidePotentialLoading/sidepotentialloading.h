#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory.h>
#include <vector>
#include <fstream>
#include <string>
#include "DriverBeam/driverbeam.h"
#include "Lattice/UnstructuredLattice/unstructuredlattice.h"
#include "DataOutput/datapackethandler.h"
#include "FrictionSystem/frictionsystem.h"

class SpringFriction;
class PotentialPusher;
class UnstructuredLattice;
class Parameters;

class SidePotentialLoading : public FrictionSystem
{
public:
    SidePotentialLoading(std::shared_ptr<Parameters> parameters);
    ~SidePotentialLoading();
    void startDriving(double tInit) override;

protected:
    int m_pusherStartHeight;
    int m_pusherEndHeight;
};

#endif // SIDEPOTENTIALLOADING_H
