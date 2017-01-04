#include "datapackethandler.h"
#include <iostream>

DataPacketHandler::DataPacketHandler(std::string outputFolder)
{
    if (outputFolder.back() != '/') {
        std::cerr << "Forgot final '/'" << std::endl;
        outputFolder += '/';
    }
    outfileNames.push_back(outputFolder + "/node_position_interface.bin");
    outfileNames.push_back(outputFolder + "/node_velocity_interface.bin");
    outfileNames.push_back(outputFolder + "/node_springs_attached_interface.bin");
    outfileNames.push_back(outputFolder + "/node_position_all.bin");
    outfileNames.push_back(outputFolder + "/node_velocity_all.bin");
    outfileNames.push_back(outputFolder + "/node_total_energy_all.bin");
    outfileNames.push_back(outputFolder + "/node_total_force_all.bin");
    outfileNames.push_back(outputFolder + "/pusher_force.bin");
    outfileNames.push_back(outputFolder + "/normal_force.bin");

    for (std::string & filename : outfileNames) {
        outfiles.push_back(std::make_shared<std::ofstream>(filename,
                                                           std::ios::out | std::ios::binary));
    }
}

DataPacketHandler::~DataPacketHandler()
{
    outfiles.clear();
}

void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
    {
        switch (packet.id())
        {
        case DataPacket::dataId::NODE_POSITION_INTERFACE:
            //std::cout << "Writing position" << std::endl;
            outfiles[0]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_VELOCITY_INTERFACE:
            //std::cout << "Writing velocity" << std::endl;
            outfiles[1]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE:
            //std::cout << "Writing nr of attached springs" << std::endl;
            outfiles[2]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_POSITION_ALL:
            outfiles[3]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_VELOCITY_ALL:
            outfiles[4]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_TOTAL_ENERGY_ALL:
            outfiles[5]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_TOTAL_FORCE_ALL:
            outfiles[6]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::PUSHER_FORCE:
            //std::cout << "Writing pusher force" << std::endl;
            outfiles[7]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NORMAL_FORCE:
            //std::cout << "Writing normal force" << std::endl;
            outfiles[8]->write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        default:
            std::cerr << "Unknown data packet" << std::endl;
            break;
        }
    }
}
