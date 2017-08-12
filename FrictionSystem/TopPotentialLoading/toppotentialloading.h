#ifndef TOPPOTENTIALLOADING_H
#define TOPPOTENTIALLOADING_H

#include <sstream>
#include <memory>
#include "FrictionSystem/frictionsystem.h"
#include "DriverBeam/driverbeam.h"
#include "DataOutput/datapacket.h"

class TopPotentialLoading : public FrictionSystem
{
public:
    TopPotentialLoading(std::shared_ptr<Parameters> parameters);
    virtual ~TopPotentialLoading();
    double totalDriverForce()         const override {return -m_driverBeam->totalShearForce();}
    size_t numberOfNodes()            const override;
    void   startDriving(double tInit)       override;
    std::vector<DataPacket> getDriverPackets(int timestep, double time) const override {return m_driverBeam->getDataPackets(timestep, time);};
    std::shared_ptr<DriverBeam> m_driverBeam;
};


#endif /* TOPPOTENTIALLOADING_H */
