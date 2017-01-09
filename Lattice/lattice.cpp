#include "lattice.h"
#include <sstream>
#include <omp.h>
#include <cmath>
#include <iostream>

Lattice::Lattice()
{

}

Lattice::~Lattice()
{
}


std::string Lattice::xyzString()
{
    std::stringstream xyz;
    xyz << nodes.size() << "\n\n";
    for (auto & node : nodes)
    {
        vec3 r = node->r()+node->r_offset();
        double phi = node->phi();
        xyz  << r[0] << " " << r[1] << " " << r[2] << " " << cos(phi) << " " << sin(phi) << " " << 0 << "\n";
    }

//    std::cout << xyz.str() << endl;
    return xyz.str();
}

void Lattice::step(double dt)
{
    omp_set_num_threads(4);
#pragma omp flush(dt)
    //for (auto & node : nodes){
    //    node->vvstep1(dt);
    //}

#pragma omp parallel for
    for (int i = 0; i<nodes.size(); i++)
    {
//        double dt2 = dt;
        nodes[i]->vvstep1(dt);
    }

    m_t += dt*0.5;

//    for (auto & node : nodes){
//        node->updateForcesAndMoments();
//    }
#pragma omp parallel for
    for (int i = 0; i<nodes.size(); i++)
    {

        nodes[i]->updateForcesAndMoments();
    }


//    for (auto & node : nodes){
//        node->vvstep2(dt);
//    }

#pragma omp parallel for
    for (int i = 0; i<nodes.size(); i++)
    {
//        double dt2 = dt;
        nodes[i]->vvstep2(dt);
    }
    m_t += dt*0.5;
}

