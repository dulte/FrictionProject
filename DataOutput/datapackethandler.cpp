#include "datapackethandler.h"
#include <iostream>

DataPacketHandler::DataPacketHandler(std::string outputFolder)
{
    if (outputFolder.back() != '/') {
        std::cerr << "Forgot final '/'" << std::endl;
        outputFolder += '/';
    }
    outfileNodePositionInterface.open(outputFolder + "/node_position_interface.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodeVelocityInterface.open(outputFolder + "/node_velocity_interface.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodeSpringsAttachedInterface.open(outputFolder + "/node_springs_attached_interface.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodePositionAll.open(outputFolder + "/node_position_all.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodeVelocityAll.open(outputFolder + "/node_velocity_all.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodeTotalEnergyAll.open(outputFolder + "/node_total_energy_all.bin",
                                      std::ios::out | std::ios::binary);
    outfileNodeTotalForceAll.open(outputFolder + "/node_total_force_all.bin",
                                      std::ios::out | std::ios::binary);
    outfilePusherForce.open(outputFolder + "/pusher_force.bin",
                                  std::ios::out | std::ios::binary);
    outfileNormalForce.open(outputFolder + "/normal_force.bin",
                            std::ios::out | std::ios::binary);
}

DataPacketHandler::~DataPacketHandler()
{
    outfileNodePositionInterface.close();
    outfileNodeVelocityInterface.close();
    outfileNodeSpringsAttachedInterface.close();
    outfileNodePositionAll.close();
    outfileNodeVelocityAll.close();
    outfileNodeTotalEnergyAll.close();
    outfileNodeTotalForceAll.close();
    outfilePusherForce.close();
    outfileNormalForce.close();
}

void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
    {
        switch (packet.id())
        {
        case DataPacket::dataId::NODE_POSITION_INTERFACE:
            //std::cout << "Writing position" << std::endl;
            outfileNodePositionInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_VELOCITY_INTERFACE:
            //std::cout << "Writing velocity" << std::endl;
            outfileNodeVelocityInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE:
            //std::cout << "Writing nr of attached springs" << std::endl;
            outfileNodeSpringsAttachedInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_POSITION_ALL:
            outfileNodePositionAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_VELOCITY_ALL:
            outfileNodeVelocityAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_TOTAL_ENERGY_ALL:
            outfileNodeTotalEnergyAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NODE_TOTAL_FORCE_ALL:
            outfileNodeTotalForceAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::PUSHER_FORCE:
            //std::cout << "Writing pusher force" << std::endl;
            outfilePusherForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        case DataPacket::dataId::NORMAL_FORCE:
            //std::cout << "Writing normal force" << std::endl;
            outfileNormalForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
    }
}
