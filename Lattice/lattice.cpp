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
    double E; double nu; double d; double hZ;
    parameters->get("E", E);
    parameters->get("nu", nu);
    parameters->get("d", d);
    parameters->get("hZ", hZ);
    return std::make_shared<LatticeInfo>(E, nu, d, hZ);
}


std::shared_ptr<Node> Lattice::newNode(std::shared_ptr<Parameters>  parameters,
                                       std::shared_ptr<LatticeInfo> latticeInfo,
                                       double x, double y) {
    double d; double density; double hZ;
    parameters->get("d", d);
    parameters->get("density", density);
    parameters->get("hZ", hZ);
    double mass = density * d * d * hZ/ 4* pi;
    double momentOfInertia = d*d / 8;
    vec3 pos(x, y, 0);
    std::shared_ptr<Node> node = std::make_shared<Node>(pos, mass, momentOfInertia, latticeInfo);
    return node;
}
