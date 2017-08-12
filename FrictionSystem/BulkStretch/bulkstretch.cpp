#include <memory>
#include <cmath>
#include "bulkstretch.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"

#define pi 3.14159265358979323

BulkStretch::BulkStretch(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
    nx = 21;
    ny = 21;
    force = 10000;
    m_lattice = std::make_shared<TriangularLattice>();
    m_lattice->populate(parameters, nx, ny);
    double d           = parameters->get<double>("d");
    double density     = parameters->get<double>("density");
    hZ                 = parameters->get<double>("hZ");
    const double kappa = m_lattice->latticeInfo->kappa_n();
    const double mass  = density*d*d*hZ/4.0 * pi;
    const double eta   = sqrt(0.1*mass*kappa);
    for (int i = 0; i<ny; i++)
    {
        std::shared_ptr<ConstantForce> myModifier = std::make_shared<ConstantForce>(vec3(-force, 0, 0));
        m_lattice->nodes[nx*i]->addModifier(std::move(myModifier));
        std::shared_ptr<ConstantForce> myModifier2 = std::make_shared<ConstantForce>(vec3(force, 0, 0));
        m_lattice->nodes[nx*i+nx-1]->addModifier(std::move(myModifier2));
    }
    for (auto & node : m_lattice->nodes)
    {
        std::shared_ptr<RelativeVelocityDamper> myDamper = std::make_shared<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(myDamper));
        std::shared_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_shared<AbsoluteOmegaDamper>(eta/40.0);
        node->addModifier(std::move(omegaDamper));
    }

    auto bottomLeftCorner  = m_lattice->nodes[0]->r();
    auto topLeftCorner     = m_lattice->nodes[nx*(ny-1)]->r();
    auto topRightCorner    = m_lattice->nodes[ny*nx-1]->r();
    // auto bottomRightCorner = m_lattice->nodes[nx]->r();
    bottomLeftCorner.print();
    topLeftCorner.print();
    topRightCorner.print();
    initialxLength           = topRightCorner.x() - topLeftCorner.x();
    initialyLength           = topLeftCorner.y() - bottomLeftCorner.y();
    // initialArea              = initalxLength * initialyLength;
}

void BulkStretch::postProcessing(){
    auto bottomLeftCorner  = m_lattice->nodes[0]->r();
    auto topLeftCorner     = m_lattice->nodes[nx*(ny-1)]->r();
    auto topRightCorner    = m_lattice->nodes[ny*nx-1]->r();
    // auto bottomRightCorner = m_lattice->nodes[nx]->r();
    bottomLeftCorner.print();
    topLeftCorner.print();
    topRightCorner.print();
    // bottomRightCorner.print();
    const double finalxLength = topRightCorner.x() - topLeftCorner.x();
    const double finalyLength = topLeftCorner.y() - bottomLeftCorner.y();
    const double crossSection = hZ*finalyLength;// finalxLength * finalyLength;
    const double strain       = (finalxLength-initialxLength)/initialxLength;
    const double stress       = force/crossSection;
    const double E = force*(initialxLength)/(crossSection*(finalxLength-initialxLength));
    std::cout << "New x length:    " << finalxLength << " m \n"
              << "New y length:    " << finalyLength << " m \n"
              << "Cross section:   " << crossSection << " m² \n"
              << "Strain:          " << strain << "\n"
              << "Stress:          " << stress << " N/m²\n"
              << "Young's modulus: " << E/1e9 << " GPa" << std::endl;
}
