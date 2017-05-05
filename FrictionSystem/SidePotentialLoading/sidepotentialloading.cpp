#include <iostream>
#include <cmath>
#include <ForceModifier/ConstantForce/constantforce.h>
#include <ForceModifier/PotentialSurface/potentialsurface.h>
#include <ForceModifier/PotentialPusher/potentialpusher.h>
#include <ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h>
#include <ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "FrictionInfo/frictioninfo.h"
#include "InputManagement/ConfigReader/configreader.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include "sidepotentialloading.h"


#define pi 3.14159265358979323

SidePotentialLoading::SidePotentialLoading(std::shared_ptr<Parameters> spParameters)
{
    // Set all member variables
    m_k                    = spParameters->m_k;
    m_pusherStartHeight    = spParameters->m_pusherStartHeight;
    m_pusherEndHeight      = spParameters->m_pusherEndHeight;
    m_vD                   = spParameters->m_vD;

    // Set the parameters used in this type name(args) const;unction
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
    const int numTop             = lattice->topNodes.size();
    const int numBottom          = lattice->bottomNodes.size();
    const double mass            = density*d*d*hZ/4.0 * pi;
    const double eta             = sqrt(0.1*mass*m_k);

    lattice = std::make_unique<TriangularLatticeWithGrooves>();
    lattice->populate(nx, ny, d, E, nu, hZ, density, grooveHeight, grooveSize);

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>(spParameters);

    for (int i = 0; i<numTop; i++)
    {
        std::unique_ptr<ConstantForce> myForce = std::make_unique<ConstantForce>(vec3(0, -topLoadingForce, 0));
        lattice->topNodes[i]->addModifier(std::move(myForce));
    }

    // Looks like debugging lines
    // std::cout << numBottom << std::endl;
    // std::cout << " noe " << normalForceDist << std::endl;
    // std::cout << "Top: " << -topLoadingForce/numTop << endl;
    for (int i = 0; i<numBottom; i++)
    {
            std::shared_ptr<SpringFriction> mySpringFriction = std::make_shared<SpringFriction>(frictionInfo);
            frictionElements.push_back(mySpringFriction);
            lattice->bottomNodes[i]->addModifier(std::move(mySpringFriction));
    }

    for (auto & node : lattice->nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> myDamper = std::make_unique<RelativeVelocityDamper>(eta);
        node->addModifier(std::move(myDamper));
        // TODO: Why is there a magic number 1e-5 here?
        std::unique_ptr<AbsoluteOmegaDamper> myOmegaDamper = std::make_unique<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(myOmegaDamper));
    }
}

SidePotentialLoading::~SidePotentialLoading()
{
    outfileParameters.close();

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
    {
        frictionElement->isLockSprings = isLock;
    }
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
