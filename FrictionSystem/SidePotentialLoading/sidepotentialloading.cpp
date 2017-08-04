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

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> parameters)
{
    // Set all member variables
    m_parameters           = parameters;
    m_pusherStartHeight    = parameters->get<int>("pusherStartHeight");
    m_pusherEndHeight      = parameters->get<int>("pusherEndHeight");
    m_vD                   = parameters->get<double>("vD");
    m_snapshotBeginTime    = parameters->get<int>("snapshotstart");
    m_snapshotBufferTime   = parameters->get<int>("snapshotbuftime");

    int nx                 = parameters->get<int>("nx");
    double topLoadingForce = parameters->get<double>("fn");
    double d               = parameters->get<double>("d");
    double density         = parameters->get<double>("density");
    double hZ              = parameters->get<double>("hZ");
    double relVelDampCoeff = parameters->get<double>("relVelDampCoeff");
    const double mass      = density*d*d*hZ/4.0 * pi;

    m_dataHandler = make_unique<DataPacketHandler>(parameters->get<std::string>("outputpath"), parameters);

    m_lattice = std::make_shared<UnstructuredLattice>();
    m_lattice->populate(parameters);

    const double kappa = m_lattice->latticeInfo->kappa_n();
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
    m_driverBeam->startDriving();
}

void SidePotentialLoading::isLockFrictionSprings(bool isLock)
{
    for (auto & frictionElement : frictionElements)
        frictionElement->isLockSprings = isLock;
}

void SidePotentialLoading::step(double step, unsigned int timestep){
    m_lattice->step(step);
    m_currentPackets = getDataPackets(timestep, timestep*step);
    m_dataHandler->step(m_currentPackets);

    if(m_dataHandler->doDumpXYZ(timestep)){
        // xyzString() is an expensive function, so it will
        // only be run if necessary
        m_dataHandler->dumpXYZ(xyzString(step*timestep));
    }
    if(doDumpSnapshot(step, timestep))
        m_dataHandler->dumpSnapshot(m_snapshotPackets, m_snapshotxyz);
}

bool SidePotentialLoading::doDumpSnapshot(double step, unsigned int timestep){
    if(timestep < m_snapshotBeginTime)
        return false;

    const double driverforce = m_driverBeam->totalShearForce();
    if (driverforce > m_maxRecordedDriveForce){
        m_maxRecordedDriveForce = driverforce;
        m_snapshotPackets = m_currentPackets;
        m_snapshotxyz = xyzString(timestep*step);
        m_newMaximum = true;
    }
    if(m_newMaximum && (timestep-m_snapshotBeginTime)%m_snapshotBufferTime == 0){
        m_newMaximum = false;
        return true;
    } else
        return false;
}

std::vector<DataPacket> SidePotentialLoading::getDataPackets(int timestep, double time)
{
    // Get the data packets from the lattice
    std::vector<DataPacket> packets = m_lattice->getDataPackets(timestep, time);

    // Get the data from the friction elements
    DataPacket attachedSprings = DataPacket(DataPacket::dataId::INTERFACE_ATTACHED_SPRINGS, timestep, time);
    DataPacket normalForce     = DataPacket(DataPacket::dataId::INTERFACE_NORMAL_FORCE, timestep, time);
    DataPacket shearForce      = DataPacket(DataPacket::dataId::INTERFACE_SHEAR_FORCE, timestep, time);
    for (auto & frictionElement : frictionElements) {
        attachedSprings.push_back(frictionElement->m_numSpringsAttached);
        normalForce.push_back(frictionElement->m_normalForce);
        shearForce.push_back(frictionElement->m_shearForce);
    }
    packets.push_back(attachedSprings);
    packets.push_back(normalForce);
    packets.push_back(shearForce);

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


std::string SidePotentialLoading::xyzString(double time) const
{
    std::stringstream xyz;
    xyz << m_lattice->normalNodes.size() + m_lattice->topNodes.size() +
        m_lattice->bottomNodes.size() + m_lattice->leftNodes.size() +
        m_driverBeam->m_nodes.size() << "\n" << "Time: " << time << "\n";

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
    ss << c << " " << r[0] << " " << r[1] << " " << f[0] << " " << f[1] << '\n';
    return ss.str();
}
