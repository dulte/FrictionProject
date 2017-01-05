#ifndef DATAPACKETHANDLER_H
#define DATAPACKETHANDLER_H

#include <fstream>
#include <memory>
#include "datapacket.h"
#include "InputManagement/ConfigReader/configreader.h"

class DataPacketHandler
{
public:
    DataPacketHandler(std::string outputfolder, ConfigReader *input);
    ~DataPacketHandler();
    void step(std::vector<DataPacket>);
    void dumpXYZ(std::string xyz);

private:
    ConfigReader *m_input;

    std::ofstream m_ofNodePositionInterface;
    std::ofstream m_ofNodeVelocityInterface;
    std::ofstream m_ofNodeSpringsAttachedInterface;
    std::ofstream m_ofNodePositionAll;
    std::ofstream m_ofNodeVelocityAll;
    std::ofstream m_ofTotalEnergyAll;
    std::ofstream m_ofTotalForceAll;
    std::ofstream m_ofPusherForce;
    std::ofstream m_ofNormalForce;
    std::ofstream m_ofShearForce;
    std::ofstream m_ofXYZ;

    bool m_writeNodePositionInterface = true;
    bool m_writeNodeVelocityInterface = true;
    bool m_writeNodeSpringsAttachedInterface = true;
    bool m_writeNodePositionAll = true;
    bool m_writeNodeVelocityAll = true;
    bool m_writeTotalEnergyAll = true;
    bool m_writeTotalForceAll = true;
    bool m_writePusherForce = true;
    bool m_writeNormalForce = true;
    bool m_writeShearForce = true;
    bool m_writeXYZ = true;
};

#endif // DATAPACKETHANDLER_H
