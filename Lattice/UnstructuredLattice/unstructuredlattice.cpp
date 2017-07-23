#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "NodeInfo/nodeinfo.h"
#include "unstructuredlattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "InputManagement/LatticeScanner/latticescanner.h"


void UnstructuredLattice::populate(std::shared_ptr<Parameters> parameters){
    m_nx = parameters->m_nx;
    m_ny = parameters->m_ny;
    m_d  = parameters->m_d;
    latticeInfo = latticeInfoFromParameters(parameters);

    LatticeScanner scanner = LatticeScanner(parameters,
                                            latticeInfo);

    scanner.scan();
    // Confirm that the scanner has indeed scanned a lattice
    if (!scanner.hasNodes())
        throw std::runtime_error("Tried to populate from empty scanner");

    // Construct the lattice
    bottomNodes = scanner.m_bottomNodes;
    topNodes    = scanner.m_topNodes;
    leftNodes   = scanner.m_leftNodes;
    normalNodes = scanner.m_normalNodes;
    nodes       = scanner.m_nodes;
    connectNodes();
}

void UnstructuredLattice::connectNodes(){
    for (auto & node : nodes)
    {
        node->setLattice(shared_from_this());
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < m_d*1.01 && node->distanceTo(*node2) > m_d*0.01)
                node->connectToNode(node2);
        }
    }
}


std::vector<DataPacket> UnstructuredLattice::getDataPackets(int timestep, double time)
{
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket position_interface_packet = DataPacket(DataPacket::dataId::NODE_POSITION_INTERFACE, timestep, time);
    DataPacket velocity_interface_packet = DataPacket(DataPacket::dataId::NODE_VELOCITY_INTERFACE, timestep, time);
    DataPacket num_springs_attached_interface_packet = DataPacket(DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE, timestep, time);
    DataPacket position_all = DataPacket(DataPacket::dataId::NODE_POSITION_ALL, timestep, time);
    DataPacket velocity_all = DataPacket(DataPacket::dataId::NODE_VELOCITY_ALL, timestep, time);
//    DataPacket energy_all = DataPachet(DataPacket::dataId::NODE_TOTAL_ENERGY_ALL,timestep,time);

    for (std::shared_ptr<Node> node : bottomNodes)
    {
        position_interface_packet.push_back(node->r().x());
        position_interface_packet.push_back(node->r().y());
        velocity_interface_packet.push_back(node->v().x());
        velocity_interface_packet.push_back(node->v().y());
    }

    for (std::shared_ptr<Node> node : nodes) {
        position_all.push_back(node->r().x());
        position_all.push_back(node->r().y());
        velocity_all.push_back(node->v().x());
        velocity_all.push_back(node->v().y());
    }
    packetvec.push_back(position_interface_packet);
    packetvec.push_back(velocity_interface_packet);
    packetvec.push_back(position_all);
    packetvec.push_back(velocity_all);
    return packetvec;
}
