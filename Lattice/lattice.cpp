#include <omp.h>
#include "lattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "LatticeInfo/latticeinfo.h"

#define pi 3.14159265358979323

Lattice::Lattice()
{

}

Lattice::~Lattice()
{
}


void Lattice::step(double dt)
{
    omp_set_num_threads( 16 );
#pragma omp flush(dt)

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep(dt);
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
        nodes[i]->vvstep(dt);
    }
    m_t += dt*0.5;
}
std::shared_ptr<LatticeInfo> Lattice::latticeInfoFromParameters(std::shared_ptr<Parameters> parameters){
    return std::make_shared<LatticeInfo>(parameters->m_E, parameters->m_nu,
                                         parameters->m_d, parameters->m_hZ);
}


std::shared_ptr<Node> Lattice::newNode(std::shared_ptr<Parameters>  parameters,
                                       std::shared_ptr<LatticeInfo> latticeInfo,
                                       double x, double y) {
    double mass = parameters->m_density * parameters->m_d * parameters->m_d *
        parameters->m_hZ / 4 * pi;
    double momentOfInertia = parameters->m_d * parameters->m_d / 8;
    vec3 pos(x, y, 0);
    std::shared_ptr<Node> node = std::make_shared<Node>(pos, mass, momentOfInertia, latticeInfo);
    return node;
}
