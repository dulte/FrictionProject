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
        std::shared_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_shared<AbsoluteOmegaDamper>(1e-5);
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
    const double crossSection = hZ*initialyLength;
    const double strain       = (finalxLength - initialxLength)/initialxLength; // Axial strain
    const double strainTrans  = (finalyLength - initialyLength)/initialyLength; // Transverse strain
    const double stress       = 21*force/crossSection;
    const double E = stress/strain * sqrt(3);
    const double nu = -strainTrans/strain;
    const double G = E/(2*(1+nu));
    const double K = E/(3*(1-2*nu));
    const double M = K + 4*G/3; // Elastic moduli P-wave modulus
    const double rho = m_parameters->get<double>("density");
    const double vp = sqrt(M/rho);
    const double actualE = m_parameters->get<double>("E") * sqrt(3);
    const double actualNu = m_parameters->get<double>("nu");
    const double actualG = actualE/(2*(1+nu));
    const double actualK = actualE/(3*(1-2*nu));
    const double actualM = actualK + 4*actualG/3;
    const double actualVp = sqrt(actualM/rho);
    std::cout << "Final x length:                 " << finalxLength << " m \n"
              << "Final y length:                 " << finalyLength << " m \n"
              << "Inital x length:               " << initialxLength << " m \n"
              << "delta " << finalxLength - initialxLength << "m \n"
              << "Cross section:                " << crossSection << " m² \n"
              << "Axial strain:                 " << strain << "\n"
              << "Transverse strain:            " << strainTrans << "\n"
              << "Stress:                       " << stress << " N/m²\n"
              << "Young's modulus:              " << E/1e9 << " GPa should be " << actualE/1e9 << " GPa\n"
              << "Poisson's Ratio:              " << nu << ", should be " << actualNu << "\n"
              << "Bulk Modulus:                 " << G/1e9 << " GPa, should be " << actualG/1e9 << " GPa\n"
              << "Elastic Moduli P-wave modulus " << M/1e9 << " GPa, should be " << actualM/1e9 << " GPa\n"
              << "Velocity of P-waves:          " << vp << " m/s, should be " << actualVp << " m/s\n";
}
