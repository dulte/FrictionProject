#include <memory>
#include "toppotentialloading.h"
#include "Node/node.h"
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/PotentialPusher/potentialpusher.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"
#include "ForceModifier/SpringFriction/springfriction.h"
#include "FrictionInfo/frictioninfo.h"
#include "Lattice/UnstructuredLattice/unstructuredlattice.h"
#include "Lattice/lattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "DataOutput/DataDumper/datadumper.h"

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
TopPotentialLoading::TopPotentialLoading(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
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
    const double alpha     = eta/parameters->get<double>("absDampCoeff");

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

    // Add the driver
    m_driverBeam = std::make_shared<DriverBeam>(m_parameters, m_lattice);
    m_driverBeam->attachToLattice();
    m_lattice->nodes.push_back(m_driverBeam);
}

TopPotentialLoading::~TopPotentialLoading() {}

void TopPotentialLoading::startDriving(double tInit){
    m_isDriving = true;
    m_driverBeam->startDriving();
    for (auto& node: m_driverBeam->m_nodes){
        std::shared_ptr<PotentialPusher> pusher = std::make_shared<PotentialPusher>(m_k, m_vD, node->r().x(), tInit);
        pusherNodes.push_back(pusher);
        node->addModifier(std::move(pusher));
        m_driverNodes.push_back(node);
    }
}

size_t TopPotentialLoading::numberOfNodes() const{
    size_t topNodes = m_isDriving ? m_driverNodes.size() : m_lattice->topNodes.size();
    return m_lattice->normalNodes.size() +
        m_lattice->bottomNodes.size() + m_lattice->leftNodes.size() +
        topNodes;
}
