#include "datapackethandler.h"
#include <iostream>

DataPacketHandler::DataPacketHandler(std::string outputFolder, std::shared_ptr<Parameters> pParameters)
{
    m_pParameters                       = pParameters; // Copy
    m_writeNodePositionInterface        = pParameters->get<bool>("writeNodePositionInterface");
    m_writeNodeVelocityInterface        = pParameters->get<bool>("writeNodeVelocityInterface");
    m_writeNodeSpringsAttachedInterface = pParameters->get<bool>("writeNodeSpringsAttachedInterface");
    m_writeNodePositionAll              = pParameters->get<bool>("writeNodePositionAll");
    m_writeNodeVelocityAll              = pParameters->get<bool>("writeNodeVelocityAll");
    m_writeTotalEnergyAll               = pParameters->get<bool>("writeTotalEnergyAll");
    m_writeNodeForceAll                 = pParameters->get<bool>("writeNodeForceAll");
    m_writePusherForce                  = pParameters->get<bool>("writePusherForce");
    m_writeNormalForce                  = pParameters->get<bool>("writeNormalForce");
    m_writeShearForce                   = pParameters->get<bool>("writeShearForce");
    m_writeXYZ                          = pParameters->get<bool>("writeXYZ");
    m_writeBeamTorque                   = pParameters->get<bool>("writeBeamTorque");
    m_writeBeamShearForce               = pParameters->get<bool>("writeBeamShearForce");

    m_freqNodePositionInterface         = pParameters->get<int>("freqNodePositionInterface");
    m_freqNodeVelocityInterface         = pParameters->get<int>("freqNodeVelocityInterface");
    m_freqNodeSpringsAttachedInterface  = pParameters->get<int>("freqNodeSpringsAttachedInterface");
    m_freqNodePositionAll               = pParameters->get<int>("freqNodePositionAll");
    m_freqNodeVelocityAll               = pParameters->get<int>("freqNodeVelocityAll");
    m_freqTotalEnergyAll                = pParameters->get<int>("freqTotalEnergyAll");
    m_freqNodeForceAll                  = pParameters->get<int>("freqNodeForceAll");
    m_freqPusherForce                   = pParameters->get<int>("freqPusherForce");
    m_freqNormalForce                   = pParameters->get<int>("freqNormalForce");
    m_freqShearForce                    = pParameters->get<int>("freqShearForce");
    m_freqXYZ                           = pParameters->get<int>("freqXYZ");
    m_freqBeamShearForce                = pParameters->get<int>("freqBeamShearForce");
    m_freqBeamTorque                    = pParameters->get<int>("freqBeamTorque");

    if (outputFolder.back() != '/') {
        std::cerr << "Forgot final '/'" << std::endl;
        outputFolder += '/';
    }
    m_ofNodePositionInterface.open(outputFolder        + "/node_position_interface.bin",         std::ios::out | std::ios::binary);
    m_ofNodeVelocityInterface.open(outputFolder        + "/node_velocity_interface.bin",         std::ios::out | std::ios::binary);
    m_ofNodeSpringsAttachedInterface.open(outputFolder + "/node_springs_attached_interface.bin", std::ios::out | std::ios::binary);
    m_ofNodePositionAll.open(outputFolder              + "/node_position_all.bin",               std::ios::out | std::ios::binary);
    m_ofNodeVelocityAll.open(outputFolder              + "/node_velocity_all.bin",               std::ios::out | std::ios::binary);
    m_ofTotalEnergyAll.open(outputFolder               + "/node_total_energy_all.bin",           std::ios::out | std::ios::binary);
    m_ofNodeForceAll.open(outputFolder                 + "/node_force_all.bin",                  std::ios::out | std::ios::binary);
    m_ofPusherForce.open(outputFolder                  + "/pusher_force.bin",                    std::ios::out | std::ios::binary);
    m_ofNormalForce.open(outputFolder                  + "/normal_force.bin",                    std::ios::out | std::ios::binary);
    m_ofShearForce.open(outputFolder                   + "/shear_force.bin",                     std::ios::out | std::ios::binary);
    m_ofXYZ.open(outputFolder                          + "/positions.xyz",                       std::ofstream::out);
    m_ofBeamShearForce.open(outputFolder               + "/beam_shear_force.bin",                std::ios::out | std::ios::binary);
    m_ofBeamTorque.open(outputFolder                   + "/beam_torque.bin",                     std::ios::out | std::ios::binary);

    // Check that the directory is writable
    if(!m_ofNodePositionInterface.is_open())
        throw runtime_error("Could not open output files");
}

DataPacketHandler::~DataPacketHandler()
{
    m_ofNodePositionInterface.close();
    m_ofNodeVelocityInterface.close();
    m_ofNodeSpringsAttachedInterface.close();
    m_ofNodePositionAll.close();
    m_ofNodeVelocityAll.close();
    m_ofTotalEnergyAll.close();
    m_ofNodeForceAll.close();
    m_ofPusherForce.close();
    m_ofNormalForce.close();
    m_ofShearForce.close();
    m_ofXYZ.close();
}

void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
    {
        switch(packet.id()){
        case DataPacket::dataId::NODE_POSITION_INTERFACE:
        {
            if(m_writeNodePositionInterface && packet.timestep()%m_freqNodePositionInterface == 0)
                m_ofNodePositionInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_VELOCITY_INTERFACE:
        {
            if(m_writeNodeVelocityInterface && packet.timestep()%m_freqNodeVelocityInterface == 0)
                m_ofNodeVelocityInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE:
        {
            if(m_writeNodeSpringsAttachedInterface && packet.timestep()%m_freqNodeSpringsAttachedInterface == 0)
                m_ofNodeSpringsAttachedInterface.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_POSITION_ALL:
        {
            if(m_writeNodePositionAll && packet.timestep()%m_freqNodePositionAll == 0)
                m_ofNodePositionAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_VELOCITY_ALL:
        {
            if(m_writeNodeVelocityAll && packet.timestep()%m_freqNodeVelocityAll == 0)
                m_ofNodeVelocityAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_TOTAL_ENERGY_ALL:
        {
            if(m_writeTotalEnergyAll && packet.timestep()%m_freqTotalEnergyAll == 0)
                m_ofTotalEnergyAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NODE_FORCE_ALL:
        {
            if(m_writeNodeForceAll && packet.timestep()%m_freqNodeForceAll == 0)
                m_ofNodeForceAll.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::PUSHER_FORCE:
        {
            if(m_writePusherForce && packet.timestep()%m_freqPusherForce == 0)
                m_ofPusherForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::NORMAL_FORCE:
        {
            if(m_writeNormalForce && packet.timestep()%m_freqNormalForce == 0)
                m_ofNormalForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::SHEAR_FORCE:
        {
            if(m_writeShearForce && packet.timestep()%m_freqShearForce == 0)
                m_ofShearForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::BEAM_TORQUE: {
            if (m_writeBeamTorque && packet.timestep()%m_freqBeamTorque == 0)
                m_ofBeamTorque.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        case DataPacket::dataId::BEAM_SHEAR_FORCE: {
            if (m_writeBeamShearForce && packet.timestep()%m_freqBeamShearForce == 0)
                m_ofBeamShearForce.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
            break;
        }
        default: {
            std::cerr << "Unknown data packet sent" << std::endl;
        }
        }
    }
}

void DataPacketHandler::dumpXYZ(const SidePotentialLoading& system, int timestep){
    if (m_writeXYZ && timestep%m_freqXYZ == 0){
        m_ofXYZ << system.xyzString();
    }
}
