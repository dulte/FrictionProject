#include "triangularlattice.h"
#include <math.h>
#include <sstream>
#include "InputManagement/Parameters/parameters.h"
#include "NodeInfo/nodeinfo.h"



#define pi 3.14159265358979323

TriangularLattice::TriangularLattice()
{

}

void TriangularLattice::populate(std::shared_ptr<Parameters> parameters){
    populate(parameters, parameters->get<int>("nx"), parameters->get<int>("ny"));
}

void TriangularLattice::populate(std::shared_ptr<Parameters> parameters, int nx, int ny)
{
    m_nx = nx;
    m_ny = ny;
    const double E = parameters->get<double>("E");
    const double nu = parameters->get<double>("nu");
    const double d = parameters->get<double>("d");
    const double hZ = parameters->get<double>("hZ");
    const double density = parameters->get<double>("density");

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

            bool normal = true;
            if (j == 0)
            {
                bottomNodes.push_back(newNode);
                normal = false;
            }
            if (j == ny-1)
            {
                topNodes.push_back(newNode);
                normal = false;
            }
            if (i == 0)
            {
                leftNodes.push_back(newNode);
                normal = false;
            }
            if (normal)
                normalNodes.push_back(newNode);
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

void TriangularLattice::populateWithUnitCell(std::shared_ptr<Parameters> parameters)
{
    const double E       = parameters->get<double>("E");
    const double nu      = parameters->get<double>("nu");
    const double d       = parameters->get<double>("d");
    const double hZ      = parameters->get<double>("hZ");
    const double density = parameters->get<double>("density");
    latticeInfo          = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    for (int j = 0; j<3; j++)
    {
        for (int i = 0; i<2; i++)
        {
            double rx = i*d+(j%2)*d*cos(pi/3);
            double ry = j*d*sin(pi/3);
            vec3 pos(rx, ry,0);
            auto node = std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
            nodes.push_back(node);
            normalNodes.push_back(node);
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

void TriangularLattice::populateCantilever(std::shared_ptr<Parameters> parameters)
{
    const double E = parameters->get<double>("E");
    const double nu = parameters->get<double>("nu");
    const double d = parameters->get<double>("d");
    const double hZ = parameters->get<double>("hZ");
    const double density = parameters->get<double>("density");
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    double rx = 0;
    double ry = d;
    vec3 pos(rx, ry,0);
    auto node1 = std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
    nodes.push_back(node1);
    normalNodes.push_back(node1);

    double rx2 = d;
    double ry2 = d;
    vec3 pos2(rx2, ry2,0);
    auto node2 = std::make_shared<Node>(pos2, density*d*d*hZ/4*pi, d*d/8, latticeInfo);
    nodes.push_back(node2);
    normalNodes.push_back(node2);

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

