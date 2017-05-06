#include "triangularlatticewithgrooves.h"
#include <math.h>
//#include <QDebug>
#include <sstream>
#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include "NodeInfo/nodeinfo.h"



#define pi 3.14159265358979323

TriangularLatticeWithGrooves::TriangularLatticeWithGrooves()
{

}

void TriangularLatticeWithGrooves::populate(int nx, int ny, double d, double E, double nu, double hZ, double density){
    m_nx = nx;
    m_ny = ny;
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);
    int grooveLength = input->get("grooveSize");
    int restLength;
    int backRest;

    if (grooveLength != 0){
        restLength = m_nx%(2*int(grooveLength));
        backRest = std::ceil(restLength/2.0);

    }


    for (int j = 0; j<ny; j++)
    {
        for (int i = 0; i<nx; i++)
        {

//            double rx = i*d+(j%2)*d*cos(pi/3);
//            double ry = j*d*sin(pi/3);
//            vec3 pos(rx, ry,0);
//            std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
//            nodes.push_back(newNode);
            if (grooveLength != 0){
                if (j < m_grooveHeight)
                {
//                    if (((int(i-backRest)/int((grooveLength)))%2 == 1) || (i<backRest) || (i> m_nx - backRest)){
//                        double rx = i*d+(j%2)*d*cos(pi/3);
//                        double ry = j*d*sin(pi/3);
//                        vec3 pos(rx, ry,0);
//                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
//                        nodes.push_back(newNode);

//                        bottomNodes.push_back(newNode);

//                        if (i == 0)
//                        {
//                            leftNodes.push_back(newNode);
//                        }
//                    }

                    if ((((int(i-1)/int((grooveLength)))%2 == 0) || (i > m_nx -restLength)) && !(i == 0 && j%2 == 0)){
                        double rx = i*d+(j%2)*d*cos(pi/3);
                        double ry = j*d*sin(pi/3);
                        vec3 pos(rx, ry,0);
                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                        nodes.push_back(newNode);



                        if (j == 0){
                            bottomNodes.push_back(newNode);
                        }


                        if (i == 0)
                        {
                            leftNodes.push_back(newNode);
                        }
                    }

                    else if ((j%2 == 1) && ((i+1-1)%(2*grooveLength) == 0)){
                        double rx = i*d+(j%2)*d*cos(pi/3);
                        double ry = j*d*sin(pi/3);
                        vec3 pos(rx, ry,0);
                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                        nodes.push_back(newNode);



                        if (j == 0){
                            bottomNodes.push_back(newNode);
                        }


                        if (i == 0)
                        {
                            leftNodes.push_back(newNode);
                        }

                    }



                }

                else{
                    double rx = i*d+(j%2)*d*cos(pi/3);
                    double ry = j*d*sin(pi/3);
                    vec3 pos(rx, ry,0);
                    std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                    nodes.push_back(newNode);


                    if (j == 0){
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

            else{
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

void TriangularLatticeWithGrooves::populateSymmetric(int nx, int ny, double d, double E, double nu, double hZ, double density){


    string bottomNodeConfigTextFile = "bottomNodesConfig.txt";

    vector<int> bottomNodeConfig;

    ifstream in{bottomNodeConfigTextFile};
    unsigned int current;
    int numLines;

    in >> numLines;

    if(numLines != nx){
        cout << "number of lines in the bottomNodeConfigText does not match nx" << endl;
    }

    for( int i = 0; i < numLines; i++){
        in >> current;
        bottomNodeConfig.emplace_back(current);
    }
    in.close();

    m_nx = nx;
    m_ny = ny;
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);
    int grooveLength = input->get("grooveSize");
    int restLength;
    int backRest;




    for (int j = 0; j<ny; j++)
    {
        for (int i = 0; i<nx; i++)
        {

            if (grooveLength != 0){
                if (j < m_grooveHeight)
                {
//

                    if (bottomNodeConfig[i] == 1){
                        double rx = i*d+(j%2)*d*cos(pi/3);
                        double ry = j*d*sin(pi/3);
                        vec3 pos(rx, ry,0);
                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                        nodes.push_back(newNode);



                        if (j == 0){
                            bottomNodes.push_back(newNode);
                        }


                        if (i == 0)
                        {
                            leftNodes.push_back(newNode);
                        }
                    }

                    if((bottomNodeConfig[i] == 0 && bottomNodeConfig[i+1] == 1) && j%2 == 1){
                        double rx = i*d+(j%2)*d*cos(pi/3);
                        double ry = j*d*sin(pi/3);
                        vec3 pos(rx, ry,0);
                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                        nodes.push_back(newNode);



                        if (j == 0){
                            bottomNodes.push_back(newNode);
                        }


                        if (i == 0)
                        {
                            leftNodes.push_back(newNode);
                        }

                    }

                    if(j%2 == 1 && i == 0){

                        double rx = (i-1)*d+(j%2)*d*cos(pi/3);
                        double ry = j*d*sin(pi/3);
                        vec3 pos(rx, ry,0);
                        std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                        nodes.push_back(newNode);



                        if (j == 0){
                            bottomNodes.push_back(newNode);
                        }


                        if (i == 0)
                        {
                            leftNodes.push_back(newNode);
                        }
                    }




                }

                else{
                    double rx = i*d+(j%2)*d*cos(pi/3);
                    double ry = j*d*sin(pi/3);
                    vec3 pos(rx, ry,0);
                    std::shared_ptr<Node> newNode= std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
                    nodes.push_back(newNode);


                    if (j == 0){
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

            else{
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
    }



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


void TriangularLatticeWithGrooves::populateWithUnitCell(double d, double E, double nu, double hZ, double density){
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

void TriangularLatticeWithGrooves::populateCantilever(double d, double E, double nu, double hZ, double density){
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



void TriangularLatticeWithGrooves::setGrooveHeight(int hg){
    m_grooveHeight = hg;
}

std::vector<DataPacket> TriangularLatticeWithGrooves::getDataPackets(int timestep, double time)
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


