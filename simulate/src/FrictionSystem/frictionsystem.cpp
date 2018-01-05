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
#include "DataOutput/DataDumper/datadumper.h"
#include "frictionsystem.h"

std::string xyzNodeString(const std::string& c, const std::shared_ptr<Node>& node);

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
FrictionSystem::FrictionSystem(std::shared_ptr<Parameters> parameters) {
    // Set all member variables
    m_parameters           = parameters;
    m_vD                   = parameters->get<double>("vD");
    m_snapshotBeginTime    = parameters->get<int>("snapshotstart");
    m_snapshotBufferTime   = parameters->get<int>("snapshotbuftime");
    m_dataHandler = make_unique<DataPacketHandler>(parameters->get<std::string>("outputpath"), parameters);
}

FrictionSystem::~FrictionSystem(){};

void FrictionSystem::isLockFrictionSprings(bool isLock)
{
    for (auto & frictionElement : frictionElements)
        frictionElement->isLockSprings = isLock;
}

void FrictionSystem::step(double step, unsigned int timestep){
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

bool FrictionSystem::doDumpSnapshot(double step, unsigned int timestep){
    if(timestep < m_snapshotBeginTime)
        return false;

    const double driverforce = totalDriverForce();
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

std::vector<DataPacket> FrictionSystem::getDataPackets(int timestep, double time)
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

    // Get the data packets from the driver
    auto driverPackets = getDriverPackets(timestep, time);
    packets.insert(packets.end(), driverPackets.begin(), driverPackets.end());
    return packets;
}

std::string FrictionSystem::xyzString(double time) const
{
    std::stringstream xyz;
    xyz << numberOfNodes() << "\n"
        << "Time: " << time << "\n";

    // Write out the lattice
    for (auto & node : m_lattice->normalNodes)
        xyz << xyzNodeString("N", node);

    for (auto & node : m_lattice->bottomNodes)
        xyz << xyzNodeString("B", node);

    for (auto & node : m_lattice->topNodes)
        xyz << xyzNodeString("T", node);

    for (auto & node : m_lattice->leftNodes)
        xyz << xyzNodeString("L", node);

    for (auto & node: m_driverNodes)
        xyz << xyzNodeString("D", node);

    return xyz.str();
}

size_t FrictionSystem::numberOfNodes() const {
    return m_lattice->topNodes.size() + m_lattice->bottomNodes.size()
         + m_lattice->leftNodes.size() + m_lattice->normalNodes.size()
        + m_driverNodes.size();
}

std::vector<DataPacket> FrictionSystem::getDriverPackets(int timestep, double time) const{
    std::vector<DataPacket> packetVec = std::vector<DataPacket>();
    DataPacket pusherForce = DataPacket(DataPacket::dataId::BEAM_SHEAR_FORCE, timestep, time);
    double pusherforce = 0;
    for(auto & node: m_driverNodes){
        pusherforce += node->f().x();
    }
    pusherForce.push_back(pusherforce);
    packetVec.push_back(pusherForce);
    return packetVec;
}

double FrictionSystem::totalDriverForce() const{
    double force = 0;
    for (auto& node: m_driverNodes)
        force += node->f().x();
    return force;
}

std::string xyzNodeString(const std::string& c, const std::shared_ptr<Node>& node)
{
    std::stringstream ss;
    vec3 r = node->r();
    vec3 f = node->f();
    ss << c << " " << r[0] << " " << r[1] << " " << f[0] << " " << f[1] <<
        " " << node->phi() << " " << node->omega() << " " << node->moment() <<  '\n';
    return ss.str();
}
