#include "datapackethandler.h"
#include <iostream>

DataPacketHandler::DataPacketHandler(std::string outputFolder)
{
    outfileNodePositionInterface.open(outputFolder+"/node_position_interface.bin", std::ios::out | std::ios::binary);
}

DataPacketHandler::~DataPacketHandler()
{
    outfileNodePositionInterface.close();
}

// Dette blir en slegge foreløpig!
void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
    {
        switch (packet.id())
        {
            case DataPacket::dataId::NODE_POSITION_INTERFACE:
            outfileNodePositionInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
    }
}
