#include <memory>
#include <cmath>
#include "bulkwave.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"

#define pi 3.14159265358979323

BulkWave::BulkWave(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
    int nx = 41;
    int ny = 21;

    m_lattice = std::make_shared<TriangularLattice>();
    m_lattice->populate(parameters, nx, ny);
    double d               = parameters->get<double>("d");
    double density         = parameters->get<double>("density");
    double hZ              = parameters->get<double>("hZ");
    const double kappa     = m_lattice->latticeInfo->kappa_n();
    const double mass      = density*d*d*hZ/4.0 * pi;
    const double eta       = sqrt(0.1*mass*kappa);

    for (int i = 0; i<ny; i++)
    {
        m_lattice->nodes[i*nx]->pertubatePosition(vec3(d*0.1, 0, 0));
        m_lattice->nodes[i*nx+1]->pertubatePosition(vec3(d*0.05, 0, 0));
    }
    for (auto & node : m_lattice->nodes)
    {
        std::shared_ptr<RelativeVelocityDamper> myModifier = std::make_shared<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(myModifier));

        std::shared_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_shared<AbsoluteOmegaDamper>(eta/40.0);
        node->addModifier(std::move(omegaDamper));
    }
}
