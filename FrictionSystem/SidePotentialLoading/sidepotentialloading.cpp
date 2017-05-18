#include <iostream>
#include <cmath>
#include <ForceModifier/ConstantForce/constantforce.h>
#include <ForceModifier/PotentialSurface/potentialsurface.h>
#include <ForceModifier/PotentialPusher/potentialpusher.h>
#include <ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h>
#include <ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "ForceModifier/StraightenerForce/straightenerforce.h"
#include "FrictionInfo/frictioninfo.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include "sidepotentialloading.h"


#define pi 3.14159265358979323

std::string xyzNodeString(std::string c, const std::shared_ptr<Node>& node);

SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> spParameters)
{
    // Set all member variables
    m_k                    = spParameters->m_k;
    m_pusherStartHeight    = spParameters->m_pusherStartHeight;
    m_pusherEndHeight      = spParameters->m_pusherEndHeight;
    m_vD                   = spParameters->m_vD;

    lattice = std::make_unique<TriangularLatticeWithGrooves>();
    // Set the parameters used in this function
    const int nx                 = spParameters->m_nx;
    const int ny                 = spParameters->m_ny;
    const int grooveHeight       = spParameters->m_grooveHeight;
    const int grooveSize         = spParameters->m_grooveSize;
    const double d               = spParameters->m_d;
    const double E               = spParameters->m_E;
    const double topLoadingForce = spParameters->m_fn;
    const double density         = spParameters->m_density;
    const double hZ              = spParameters->m_hZ;
    const double nu              = spParameters->m_nu;
    const double mass            = density*d*d*hZ/4.0 * pi;
    const double eta             = sqrt(0.1*mass*m_k);

    lattice->populate(nx, ny, d, E, nu, hZ, density, grooveHeight, grooveSize);

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>(spParameters);
    m_driverBeam = std::make_unique<DriverBeam>(spParameters, lattice);

    // Add top loading force
    for (auto & node :m_driverBeam->m_attachmentNodes)
    {
        std::unique_ptr<ConstantForce> force = std::make_unique<ConstantForce>(vec3(0, -topLoadingForce, 0));
        node->addModifier(std::move(force));
    }

    // Add dampening force
    for (auto & node : lattice->bottomNodes)
    {
            std::shared_ptr<SpringFriction> springFriction = std::make_shared<SpringFriction>(frictionInfo);
            frictionElements.push_back(springFriction);
            node->addModifier(std::move(springFriction));
    }

    for (auto & node : lattice->nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> damper = std::make_unique<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(damper));
        // TODO: Why is there a magic number 1e-5 here?
        std::unique_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_unique<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(omegaDamper));
    }

    for (auto & node : m_driverBeam->nodes){
        std::unique_ptr<RelativeVelocityDamper> damper = std::make_unique<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(damper));
        // TODO: Why is there a magic number 1e-5 here?
        std::unique_ptr<AbsoluteOmegaDamper> omegaDamper = std::make_unique<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(omegaDamper));
    }
    for (auto & node : m_driverBeam->m_attachmentNodes){
        std::unique_ptr<StraightenerForce> straightenerForce = std::make_unique<StraightenerForce>(m_driverBeam->m_attachmentNodes,spParameters);
        node->addModifier(std::move(straightenerForce));
    }
}

SidePotentialLoading::~SidePotentialLoading()
{
    outfileParameters.close();
}

void SidePotentialLoading::addDriverForce(double tInit){
    auto newPusherNodes = m_driverBeam->addDriverForce(tInit);
    pusherNodes.insert(std::end(pusherNodes), std::begin(newPusherNodes), std::end(newPusherNodes));
}

void SidePotentialLoading::addPusher(double tInit)
{
    for (int j = m_pusherStartHeight; j < m_pusherEndHeight; j++)
    {
        std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(m_k, m_vD, lattice->leftNodes[j]->r().x(), tInit);
        pusherNodes.push_back(myPusher);

        lattice->leftNodes[j]->addModifier(std::move(myPusher));
    }
}

void SidePotentialLoading::isLockFrictionSprings(bool isLock)
{
    for (auto & frictionElement : frictionElements)
        frictionElement->isLockSprings = isLock;
}

// void SidePotentialLoading::dumpParameters()
// {
//     outfileParameters.open(outFileFolder+std::string("parameters.txt"));
//     std::vector<string> outputParameters = {"nx","step","nt","grooveHeight","grooveSize","pusherStartHeight","pusherEndHeight"};
//     for (string name: outputParameters){
//         outfileParameters << name << " " << input->get(name) << "\n";
//     }


//     outfileParameters << "bottomNodes" << " " << lattice->bottomNodes.size() << "\n";

//     outfileParameters.close();

// }

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
    xyz << lattice->nodes.size() + m_driverBeam->nodes.size() + lattice->bottomNodes.size() << "\n\n";

    // Write out the lattice
    for (auto & node : lattice->nodes)
        xyz << xyzNodeString("H", node);

    for (auto & node : lattice->bottomNodes)
        xyz << xyzNodeString("He", node);

    // Write out the driver beam
    for (auto & node : m_driverBeam->m_attachmentNodes)
        xyz << xyzNodeString("O", node);

    for (auto & node : m_driverBeam->m_driverNodes)
        xyz << xyzNodeString("C", node);

    return xyz.str();
}

std::string xyzNodeString(std::string c, const std::shared_ptr<Node>& node)
{
    std::stringstream ss;
    vec3 r = node->r()+node->r_offset();
    double phi = node->phi();
    ss << c << " " << r[0] << " " << r[1] << " " << r[2] << " " << cos(phi) << " " << sin(phi) << " " << 0 << "\n";
    return ss.str();
}
