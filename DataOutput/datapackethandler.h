#ifndef DATAPACKETHANDLER_H
#define DATAPACKETHANDLER_H

#include "datapacket.h"
#include <fstream>
#include <memory>

class DataPacketHandler
{
public:
    DataPacketHandler(std::string outputfolder);
    ~DataPacketHandler();
    void step(std::vector<DataPacket>);

private:
    std::vector<std::shared_ptr<std::ofstream>> outfiles;
    std::vector<std::string> outfileNames;
};

#endif // DATAPACKETHANDLER_H
