#include "Lattice/simplelattice.h"
#include <omp.h>

SimpleLattice::SimpleLattice()
{

}

SimpleLattice::~SimpleLattice()
{
}
void SimpleLattice::step(double dt)
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
