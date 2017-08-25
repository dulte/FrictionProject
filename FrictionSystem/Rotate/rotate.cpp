#include <memory>
#include "cmath"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "rotate.h"

#define pi 3.14159265358979323

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

Rotate::Rotate(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
    m_lattice = std::make_shared<TriangularLattice>();
    // m_lattice->populateCantilever(parameters);
    m_lattice->populateWithUnitCell(parameters);
    std::cout << m_lattice->nodes.size();

    double d           = parameters->get<double>("d");
    double density     = parameters->get<double>("density");
    double hZ          = parameters->get<double>("hZ");
    const double kappa = m_lattice->latticeInfo->kappa_n();
    const double mass  = density*d*d*hZ/4.0 * pi;
    const double eta   = sqrt(0.1*mass*kappa);
    const double alpha = parameters->get<double>("alpha");

    for(auto& node: m_lattice->nodes){
        std::unique_ptr<RelativeVelocityDamper> damper = make_unique<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(damper));
        std::unique_ptr<AbsoluteOmegaDamper> omegaDamper = make_unique<AbsoluteOmegaDamper>(alpha);
        node->addModifier(std::move(omegaDamper));
    }
    m_lattice->nodes[0]->pertubateRotation(pi-pi/3);
    // m_lattice->nodes[1]->pertubateRotation(pi/3);
}
