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
#include "InputManagement/LatticeScanner/latticescanner.h"
#include "sidepotentialloading.h"


#define pi 3.14159265358979323

std::string xyzNodeString(const std::string& c, const std::shared_ptr<Node>& node);

SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> parameters)
{
    // Set all member variables
    m_parameters           = parameters;
    m_k                    = parameters->get<double>("k");
    m_pusherStartHeight    = parameters->get<int>("pusherStartHeight");
    m_pusherEndHeight      = parameters->get<int>("pusherEndHeight");
    m_vD                   = parameters->get<double>("vD");

    int nx                 = parameters->get<int>("nx");
    double topLoadingForce = parameters->get<double>("fn");
    double d               = parameters->get<double>("d");
    double density         = parameters->get<double>("density");
    double hZ              = parameters->get<double>("hZ");
    const double mass      = density*d*d*hZ/4.0 * pi;
    const double eta       = sqrt(0.1*mass*m_k);

    m_lattice      = std::make_shared<UnstructuredLattice>();
    m_lattice->populate(parameters);

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
        // TODO: Why is there a magic number 1e-5 here?
        std::shared_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_shared<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(omegaDamper));
    }
    addDriver();
}

SidePotentialLoading::~SidePotentialLoading()
{
}

void SidePotentialLoading::addPusher(double tInit)
{
    if (m_lattice->leftNodes.size() <= 0)
        throw std::runtime_error("Lattice has no left nodes, and can not addPusher");

    for (int j = m_pusherStartHeight; j < m_pusherEndHeight; j++)
    {
        std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(m_k, m_vD, m_lattice->leftNodes[j]->r().x(), tInit);
        pusherNodes.push_back(myPusher);

        m_lattice->leftNodes[j]->addModifier(std::move(myPusher));
    }
}

void SidePotentialLoading::addDriver(){
    m_driverBeam = std::make_shared<DriverBeam>(m_parameters, m_lattice);
    m_driverBeam->attachToLattice();
    m_lattice->nodes.push_back(m_driverBeam);
}

void SidePotentialLoading::startDriving(){
    // auto newPusherNodes = m_driverBeam->addPushers(tInit);
    // pusherNodes.insert(std::end(pusherNodes), std::begin(newPusherNodes), std::end(newPusherNodes));
    m_driverBeam->startDriving();
}

void SidePotentialLoading::isLockFrictionSprings(bool isLock)
{
    for (auto & frictionElement : frictionElements)
        frictionElement->isLockSprings = isLock;
}

void SidePotentialLoading::step(double step){
    m_lattice->step(step);
}

std::vector<DataPacket> SidePotentialLoading::getDataPackets(int timestep, double time)
{
    // Get the data packets from the lattice
    std::vector<DataPacket> packets = m_lattice->getDataPackets(timestep, time);

    // Get the data from the friction elements
    for (auto & frictionElement : frictionElements) {
        std::vector<DataPacket> packet = frictionElement->getDataPackets(timestep, time);
        packets.insert(packets.end(), packet.begin(), packet.end());
    }

    // Get the data packets from the pusher nodes
    double pushForce = 0;
    for (auto & pusherNode : pusherNodes){
        pushForce += pusherNode->fPush;
    }
    DataPacket pusherForce = DataPacket(DataPacket::dataId::PUSHER_FORCE, timestep, time);

    pusherForce.push_back(pushForce);
    packets.push_back(pusherForce);

    // Get the data packets from the driver beam
    auto driverbeam = m_driverBeam->getDataPackets(timestep, time);
    packets.insert(packets.end(), driverbeam.begin(), driverbeam.end());
    return packets;
}


std::string SidePotentialLoading::xyzString() const
{
    std::stringstream xyz;
    xyz << m_lattice->normalNodes.size() + m_lattice->topNodes.size() +
        m_lattice->bottomNodes.size() + m_lattice->leftNodes.size() +
        m_driverBeam->m_nodes.size() << "\n\n";

    // Write out the lattice
    for (auto & node : m_lattice->normalNodes)
        xyz << xyzNodeString("N", node);

    for (auto & node : m_lattice->bottomNodes)
        xyz << xyzNodeString("B", node);

    for (auto & node : m_lattice->topNodes)
        xyz << xyzNodeString("T", node);

    for (auto & node : m_lattice->leftNodes)
        xyz << xyzNodeString("L", node);

    for (auto & node: m_driverBeam->m_nodes)
        xyz << xyzNodeString("T", node);

    return xyz.str();
}


std::string xyzNodeString(const std::string& c, const std::shared_ptr<Node>& node)
{
    std::stringstream ss;
    vec3 r = node->r();
    vec3 f = node->f();
    ss << c << " " << r[0] << " " << r[1] << " " << node->v().length() << " " << f[0] << " " << f[1] << '\n';
    return ss.str();
}
