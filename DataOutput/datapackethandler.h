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

};

#endif // DATAPACKETHANDLER_H
