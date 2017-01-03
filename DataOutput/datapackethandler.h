#ifndef DATAPACKETHANDLER_H
#define DATAPACKETHANDLER_H

#include "datapacket.h"
#include <fstream>

class DataPacketHandler
{
public:
    DataPacketHandler(std::string outputfolder);
    ~DataPacketHandler();
    void step(std::vector<DataPacket>);

private:
    std::ofstream outfileNodePositionInterface;
    std::ofstream outfileNodeVelocityInterface;
    std::ofstream outfileNodeSpringsAttachedInterface;
    std::ofstream outfileNodePositionAll;
    std::ofstream outfileNodeVelocityAll;
    std::ofstream outfileNodeTotalEnergyAll;
    std::ofstream outfileNodeTotalForceAll;
    std::ofstream outfilePusherForce;
};

#endif // DATAPACKETHANDLER_H
