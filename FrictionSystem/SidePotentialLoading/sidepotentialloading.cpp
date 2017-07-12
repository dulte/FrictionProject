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

std::string xyzNodeString(std::string c, const std::shared_ptr<Node>& node);

SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> parameters)
{
    // Set all member variables
    m_k                    = parameters->m_k;
    m_pusherStartHeight    = parameters->m_pusherStartHeight;
    m_pusherEndHeight      = parameters->m_pusherEndHeight;
    m_vD                   = parameters->m_vD;

    const double d               = parameters->m_d;
    const double density         = parameters->m_density;
    const double hZ              = parameters->m_hZ;
    const double mass            = density*d*d*hZ/4.0 * pi;
    const double eta             = sqrt(0.1*mass*m_k);
    const double topLoadingForce = parameters->m_fn;

    lattice      = std::make_shared<UnstructuredLattice>();
    m_driverBeam = std::make_shared<DriverBeam>(parameters);
    lattice->populate(parameters);

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>(parameters);

    // Add top loading force
    for (auto & node : lattice->topNodes)
    {
        std::unique_ptr<ConstantForce> force = std::make_unique<ConstantForce>(vec3(0, -topLoadingForce, 0));
        node->addModifier(std::move(force));
    }

    // Add springs
    for (auto & node : lattice->bottomNodes)
    {
            std::shared_ptr<SpringFriction> springFriction = std::make_shared<SpringFriction>(frictionInfo);
            frictionElements.push_back(springFriction);
            node->addModifier(std::move(springFriction));
    }

    // Add dampning force
    for (auto & node : lattice->nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> damper = std::make_unique<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(damper));
        // TODO: Why is there a magic number 1e-5 here?
        std::unique_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_unique<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(omegaDamper));
    }
    m_driverBeam->attachToLattice(lattice);
}

SidePotentialLoading::~SidePotentialLoading()
{
}

void SidePotentialLoading::addPusher(double tInit)
{
    if (lattice->leftNodes.size() <= 0)
        throw std::runtime_error("Lattice has no left nodes, and can not addPusher");

    for (int j = m_pusherStartHeight; j < m_pusherEndHeight; j++)
    {
        std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(m_k, m_vD, lattice->leftNodes[j]->r().x(), tInit);
        pusherNodes.push_back(myPusher);

        lattice->leftNodes[j]->addModifier(std::move(myPusher));
    }
}

void SidePotentialLoading::addDriver(double tInit){
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
    lattice->step(step);
    m_driverBeam->step(step);
}

std::vector<DataPacket> SidePotentialLoading::getDataPackets(int timestep, double time)
{
    // Get the data packets from the lattice
    std::vector<DataPacket> packets = lattice->getDataPackets(timestep, time);

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

    return packets;
}


std::string SidePotentialLoading::xyzString() const
{
    std::stringstream xyz;
    xyz << lattice->normalNodes.size() + lattice->topNodes.size() +
        lattice->bottomNodes.size() + lattice->leftNodes.size() +
        m_driverBeam->nodes.size() << "\n\n";

    // Write out the lattice
    for (auto & node : lattice->normalNodes)
        xyz << xyzNodeString("N", node);

    for (auto & node : lattice->bottomNodes)
        xyz << xyzNodeString("B", node);

    for (auto & node : lattice->topNodes)
        xyz << xyzNodeString("T", node);

    for (auto & node : lattice->leftNodes)
        xyz << xyzNodeString("L", node);

    for (auto & node: m_driverBeam->m_topNodes)
        xyz << xyzNodeString("T", node);

    return xyz.str();
}


std::string xyzNodeString(std::string c, const std::shared_ptr<Node>& node)
{
    std::stringstream ss;
    vec3 r = node->r()+node->r_offset();
    vec3 f = node->f();
    // double phi = node->phi();
    ss << c << " " << r[0] << " " << r[1] << " " << node->v().length() << " " << f[0] << " " << f[1] << '\n';
    return ss.str();
}
