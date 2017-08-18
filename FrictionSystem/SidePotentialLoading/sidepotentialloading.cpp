#include <iostream>
#include <cmath>
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/PotentialSurface/potentialsurface.h"
#include "ForceModifier/PotentialPusher/potentialpusher.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"
#include "ForceModifier/SpringFriction/springfriction.h"
#include "FrictionInfo/frictioninfo.h"
#include "Lattice/UnstructuredLattice/unstructuredlattice.h"
#include "Lattice/lattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "DataOutput/DataDumper/datadumper.h"
#include "sidepotentialloading.h"


#define pi 3.14159265358979323

SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
    m_pusherStartHeight    = parameters->get<int>("pusherStartHeight");
    m_pusherEndHeight      = parameters->get<int>("pusherEndHeight");
    int nx                 = parameters->get<int>("nx");
    double topLoadingForce = parameters->get<double>("fn");
    double d               = parameters->get<double>("d");
    double density         = parameters->get<double>("density");
    double hZ              = parameters->get<double>("hZ");
    double relVelDampCoeff = parameters->get<double>("relVelDampCoeff");
    const double mass      = density*d*d*hZ/4.0 * pi;

    m_lattice = std::make_shared<UnstructuredLattice>();
    m_lattice->populate(parameters);

    const double kappa     = m_lattice->latticeInfo->kappa_n();
    const double eta       = sqrt(0.1*mass*kappa) * relVelDampCoeff;
    const double alpha     = parameters->get<double>("alpha");

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>(parameters);

    // Add top loading force
    double N = -topLoadingForce/nx;
    for (auto & node : m_lattice->topNodes)
    {
        std::shared_ptr<ConstantForce> force = std::make_shared<ConstantForce>(vec3(0, N, 0));
        node->addModifier(std::move(force));
    }

    // Add springs
    for (auto & node : m_lattice->bottomNodes)
    {
        std::shared_ptr<SpringFriction> springFriction = std::make_shared<SpringFriction>(frictionInfo);
        frictionElements.push_back(springFriction);
        node->addModifier(std::move(springFriction));
    }

    // Add dampning force
    for (auto & node : m_lattice->nodes)
    {
        std::shared_ptr<RelativeVelocityDamper> damper = std::make_shared<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(damper));
        std::shared_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_shared<AbsoluteOmegaDamper>(alpha);
        node->addModifier(std::move(omegaDamper));
    }

    // Quick bandaid
    m_loadingForce = N;
    m_mass = mass;
    m_kappa = kappa;
    m_eta = eta;
    m_alpha = alpha;

    DataDumper dumper(parameters->get<std::string>("outputpath"));
    dumper.dumpSystem(this);
    dumper.dumpLatticeInfo(m_lattice->latticeInfo);
    dumper.dumpFrictionInfo(frictionInfo);
    dumper.dumpParameters(parameters);
    dumper.dumpLatticeStructure(m_lattice);
}

SidePotentialLoading::~SidePotentialLoading()
{
}

void SidePotentialLoading::startDriving(double tInit)
{
    double pK = m_parameters->get<double>("pK");
    if (m_lattice->leftNodes.size() <= 0)
        throw std::runtime_error("Lattice has no left nodes, and can not addPusher");

    // for (int j = m_pusherStartHeight; j < m_pusherEndHeight; j++)
    for(auto& node: m_lattice->topNodes)
    {
        // std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(pK, m_vD, m_lattice->leftNodes[j]->r().x(), tInit);
         std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(pK, m_vD, node->r().x(), tInit);
        pusherNodes.push_back(myPusher);

        // m_lattice->leftNodes[j]->addModifier(std::move(myPusher));
        node->addModifier(std::move(myPusher));
        m_driverNodes.push_back(node);
        // m_driverNodes.push_back(m_lattice->leftNodes[j]);
    }
}
