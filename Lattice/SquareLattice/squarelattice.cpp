#include "squarelattice.h"
//#include <QDebug>
#include "NodeInfo/nodeinfo.h"
#include "memory.h"

#define pi 3.14159265358979323

SquareLattice::SquareLattice()
{

}

void SquareLattice::populate(int nx, int ny, double d, double E, double nu, double hZ, double density)
{
    latticeInfo = std::make_shared<LatticeInfo>(E, nu, d, hZ);

    for (int j = 0; j<ny; j++)
    {
        for (int i = 0; i<nx; i++)
        {
            double rx = i*d;
            double ry = j*d;
            vec3 pos(rx, ry,0);
            nodes.push_back(std::make_shared<Node>(pos, density*d*d*hZ/4*pi, d*d/8, latticeInfo));
        }
    }

    for (auto & node : nodes)
    {
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < d*1.01 && node->distanceTo(*node2) > d*0.01)
            {
                node->connectToNode(node2);
            }
        }
//        qDebug() << node->numNeighbors();
    }
}
