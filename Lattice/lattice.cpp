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
    return xyz.str();
}

void Lattice::step(double dt)
{
    omp_set_num_threads(4);
#pragma omp flush(dt)

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep1(dt);
    }

    m_t += dt*0.5;

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {

        nodes[i]->updateForcesAndMoments();
    }

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep2(dt);
    }
    m_t += dt*0.5;
}
