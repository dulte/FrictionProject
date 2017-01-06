#include "triangularlattice.h"
#include <math.h>
//#include <QDebug>
#include <sstream>
#include "NodeInfo/nodeinfo.h"



#define pi 3.14159265358979323

TriangularLattice::TriangularLattice()
{

}


void TriangularLattice::populate(int nx, int ny, double d, double E, double nu, double hZ, double density)
{
    m_nx = nx;
    m_ny = ny;
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    for (int j = 0; j<ny; j++)
    {
        for (int i = 0; i<nx; i++)
        {
            double rx = i*d+(j%2)*d*cos(pi/3);
            double ry = j*d*sin(pi/3);
            vec3 pos(rx, ry,0);
            std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
            nodes.push_back(newNode);
            if (j == 0)
            {
                bottomNodes.push_back(newNode);
            }
            if (j == ny-1)
            {
                topNodes.push_back(newNode);
            }
            if (i == 0)
            {
                leftNodes.push_back(newNode);
            }
        }
    }

    // Make right part for system to be symmetric
    /*
    for(int j = 0; j<ny; j+=2)
    {
        int i = nx;
        double rx = i*d+(j%2)*d*cos(pi/3);
        double ry = j*d*sin(pi/3);
        vec3 pos(rx, ry,0);
        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
        nodes.push_back(newNode);
        if (j == 0)
        {
            bottomNodes.push_back(newNode);
        }
        if (j == ny-1)
        {
            topNodes.push_back(newNode);
        }
    }
    */

    for (auto & node : nodes)
    {
        node->setLattice(shared_from_this());
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < d*1.01 && node->distanceTo(*node2) > d*0.01)
            {
                node->connectToNode(node2);
            }
        }
    }
}

void TriangularLattice::populateWithUnitCell(double d, double E, double nu, double hZ, double density)
{
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    for (int j = 0; j<3; j++)
    {
        for (int i = 0; i<2; i++)
        {
            double rx = i*d+(j%2)*d*cos(pi/3);
            double ry = j*d*sin(pi/3);
            vec3 pos(rx, ry,0);
            nodes.push_back(std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo));
        }
    }
    double rx = -d*cos(pi/3);
    double ry = d*sin(pi/3);
    vec3 pos(rx, ry,0);
    nodes.push_back(std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo));

    for (auto & node : nodes)
    {
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < d*1.01 && node->distanceTo(*node2) > d*0.01)
            {
                node->connectToNode(node2);
            }
        }
    }
}

void TriangularLattice::populateCantilever(double d, double E, double nu, double hZ, double density)
{
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    double rx = 0;
    double ry = d;
    vec3 pos(rx, ry,0);
    nodes.push_back(std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo));

    double rx2 = d;
    double ry2 = d;
    vec3 pos2(rx2, ry2,0);
    nodes.push_back(std::make_shared<Node>(pos2, density*d*d*hZ/4*pi, d*d/8, latticeInfo));

    for (auto & node : nodes)
    {
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < d*1.01 && node->distanceTo(*node2) > d*0.01)
            {
                node->connectToNode(node2);
            }
        }
    }
}

std::vector<DataPacket> TriangularLattice::getDataPackets(int timestep, double time)
{
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket position_interface_packet = DataPacket(DataPacket::dataId::NODE_POSITION_INTERFACE, timestep, time);
    DataPacket velocity_interface_packet = DataPacket(DataPacket::dataId::NODE_VELOCITY_INTERFACE, timestep, time);
    DataPacket num_springs_attached_interface_packet = DataPacket(DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE, timestep, time);
    DataPacket position_all = DataPacket(DataPacket::dataId::NODE_POSITION_ALL, timestep, time);
    DataPacket velocity_all = DataPacket(DataPacket::dataId::NODE_VELOCITY_ALL, timestep, time);


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



