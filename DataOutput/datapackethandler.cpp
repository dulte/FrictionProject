#include "datapackethandler.h"
#include <iostream>

DataPacketHandler::DataPacketHandler(std::string outputFolder, ConfigReader *input)
{
    m_input = input;
    m_writeNodePositionInterface = m_input->get("writeNodePositionInterface") == 1;
    m_writeNodeVelocityInterface = m_input->get("writeNodeVelocityInterface") == 1;
    m_writeNodeSpringsAttachedInterface = m_input->get("writeNodeSpringsAttachedInterface") == 1;
    m_writeNodePositionAll = m_input->get("writeNodePositionAll") == 1;
    m_writeNodeVelocityAll = m_input->get("writeNodeVelocityAll") == 1;
    m_writeTotalEnergyAll = m_input->get("writeTotalEnergyAll") == 1;
    m_writeTotalForceAll = m_input->get("writeTotalForceAll") == 1;
    m_writePusherForce = m_input->get("writePusherForce") == 1;
    m_writeNormalForce = m_input->get("writeNormalForce") == 1;
    m_writeShearForce = m_input->get("writeShearForce") == 1;
    m_writeXYZ = m_input->get("writeXYZ") == 1;

    m_freqNodePositionInterface = int(m_input->get("freqNodePositionInterface"));
    m_freqNodeVelocityInterface = int(m_input->get("freqNodeVelocityInterface"));
    m_freqNodeSpringsAttachedInterface = int(m_input->get("freqNodeSpringsAttachedInterface"));
    m_freqNodePositionAll = int(m_input->get("freqNodePositionAll"));
    m_freqNodeVelocityAll = int(m_input->get("freqNodeVelocityAll"));
    m_freqTotalEnergyAll = int(m_input->get("freqTotalEnergyAll"));
    m_freqTotalForceAll = int(m_input->get("freqTotalForceAll"));
    m_freqPusherForce = int(m_input->get("freqPusherForce"));
    m_freqNormalForce = int(m_input->get("freqNormalForce"));
    m_freqShearForce = int(m_input->get("freqShearForce"));
    m_freqXYZ = int(m_input->get("freqXYZ"));

    if (outputFolder.back() != '/') {
        std::cerr << "Forgot final '/'" << std::endl;
        outputFolder += '/';
    }
    m_ofNodePositionInterface.open(outputFolder + "/node_position_interface.bin", std::ios::out | std::ios::binary);
    m_ofNodeVelocityInterface.open(outputFolder + "/node_velocity_interface.bin", std::ios::out | std::ios::binary);
    m_ofNodeSpringsAttachedInterface.open(outputFolder + "/node_springs_attached_interface.bin", std::ios::out | std::ios::binary);
    m_ofNodePositionAll.open(outputFolder + "/node_position_all.bin", std::ios::out | std::ios::binary);
    m_ofNodeVelocityAll.open(outputFolder + "/node_velocity_all.bin", std::ios::out | std::ios::binary);
    m_ofTotalEnergyAll.open(outputFolder + "/node_total_energy_all.bin", std::ios::out | std::ios::binary);
    m_ofTotalForceAll.open(outputFolder + "/node_total_force_all.bin", std::ios::out | std::ios::binary);
    m_ofPusherForce.open(outputFolder + "/pusher_force.bin", std::ios::out | std::ios::binary);
    m_ofNormalForce.open(outputFolder + "/normal_force.bin", std::ios::out | std::ios::binary);
    m_ofShearForce.open(outputFolder + "/shear_force.bin", std::ios::out | std::ios::binary);
    m_ofXYZ.open(outputFolder + "/positions.xyz", std::ofstream::out);
}

DataPacketHandler::~DataPacketHandler()
{
    m_ofNodePositionInterface.close();
    m_ofNodeVelocityInterface.close();
    m_ofNodeSpringsAttachedInterface.close();
    m_ofNodePositionAll.close();
    m_ofNodeVelocityAll.close();
    m_ofTotalEnergyAll.close();
    m_ofTotalForceAll.close();
    m_ofPusherForce.close();
    m_ofNormalForce.close();
    m_ofShearForce.close();
    m_ofXYZ.close();    
}

void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
    {
        if(packet.id() ==  DataPacket::dataId::NODE_POSITION_INTERFACE)
        {
            if(m_writeNodePositionInterface && packet.timestep()%m_freqNodePositionInterface == 0)
                m_ofNodePositionInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_VELOCITY_INTERFACE)
        {
            if(m_writeNodeVelocityInterface && packet.timestep()%m_freqNodeVelocityInterface == 0)
                m_ofNodeVelocityInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE)
        {
            if(m_writeNodeSpringsAttachedInterface && packet.timestep()%m_freqNodeSpringsAttachedInterface == 0)
                m_ofNodeSpringsAttachedInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_POSITION_ALL)
        {
            if(m_writeNodePositionAll && packet.timestep()%m_freqNodePositionAll == 0)
                m_ofNodePositionAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_VELOCITY_ALL)
        {
            if(m_writeNodeVelocityAll && packet.timestep()%m_freqNodeVelocityAll == 0)
                m_ofNodeVelocityAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_TOTAL_ENERGY_ALL)
        {
            if(m_writeTotalEnergyAll && packet.timestep()%m_freqTotalEnergyAll == 0)
                m_ofTotalEnergyAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NODE_TOTAL_FORCE_ALL)
        {
            if(m_writeTotalForceAll && packet.timestep()%m_freqTotalForceAll == 0)
                m_ofTotalForceAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::PUSHER_FORCE)
        {
            if(m_writePusherForce && packet.timestep()%m_freqPusherForce == 0)
                m_ofPusherForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() ==  DataPacket::dataId::NORMAL_FORCE)
        {
            if(m_writeNormalForce && packet.timestep()%m_freqNormalForce == 0)
                m_ofNormalForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else if(packet.id() == DataPacket::dataId::SHEAR_FORCE)
        {
            if(m_writeShearForce && packet.timestep()%m_freqShearForce == 0)
                m_ofShearForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
        }
        else{
            std::cerr << "Unknown data packet sent" << std::endl;
        }
            
    }
}

void DataPacketHandler::dumpXYZ(const std::shared_ptr<TriangularLatticeWithGrooves>& lattice, int timestep){
    if (m_writeXYZ && timestep%m_freqXYZ == 0){
        m_ofXYZ << lattice->xyzString();
    }
}
