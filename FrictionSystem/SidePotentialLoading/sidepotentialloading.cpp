#include "sidepotentialloading.h"
#include <iostream>


#include <ForceModifier/ConstantForce/constantforce.h>
#include <ForceModifier/PotentialSurface/potentialsurface.h>
#include <ForceModifier/PotentialPusher/potentialpusher.h>
#include <ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h>
#include <ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "FrictionInfo/frictioninfo.h"

#include "Lattice/SquareLattice/squarelattice.h"

SidePotentialLoading::SidePotentialLoading(int nx, int ny, double d, double E, double k, double topLoadingForce)
{
    std::string outFileFolder = "";
    outfileParameters.open(outFileFolder+std::string("parameters.txt"));

//    lattice = std::make_unique<TriangularLattice>();
    lattice = std::make_unique<TriangularLatticeWithGrooves>();


    lattice->populate(nx, ny, d, E, 0.33, 0.006, 1300);

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>();

    int numTop = lattice->topNodes.size();
    double normalForceDist = input->get("nx")/double(lattice->bottomNodes.size());
    std::cout << " noe " << normalForceDist << std::endl;
    std::cout << "Top: " << -topLoadingForce/numTop << endl;
    for (int i = 0; i<numTop; i++)
    {
        std::unique_ptr<ConstantForce> myForce = std::make_unique<ConstantForce>(vec3(0, -topLoadingForce/numTop*normalForceDist, 0));
        lattice->topNodes[i]->addModifier(std::move(myForce));
    }

    int numBottom = lattice->bottomNodes.size();
    std::cout << numBottom << std::endl;
    for (int i = 0; i<numBottom; i++)
    {
        if (i > 0)
        {
            std::shared_ptr<SpringFriction> mySpringFriction = std::make_shared<SpringFriction>(frictionInfo);
            mySpringFriction->neighbor = lattice->bottomNodes[i-1];
            frictionElements.push_back(mySpringFriction);
            lattice->bottomNodes[i]->addModifier(std::move(mySpringFriction));
        }

        if (i < (numBottom - 1))
        {
            std::shared_ptr<SpringFriction> mySpringFriction = std::make_shared<SpringFriction>(frictionInfo);
            mySpringFriction->neighbor = lattice->bottomNodes[i+1];
            frictionElements.push_back(mySpringFriction);
            lattice->bottomNodes[i]->addModifier(std::move(mySpringFriction));
        }


    }

    for (auto & node : lattice->nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> myDamper = std::make_unique<RelativeVelocityDamper>(20.0);
        node->addModifier(std::move(myDamper));
        std::unique_ptr<AbsoluteOmegaDamper> myOmegaDamper = std::make_unique<AbsoluteOmegaDamper>(1e-5);
        node->addModifier(std::move(myOmegaDamper));
    }
}

SidePotentialLoading::~SidePotentialLoading()
{
    outfileParameters.close();

}

void SidePotentialLoading::addPusher(double k, double vD, double tInit)
{
    for (int j = input->get("pusherStartHeight"); j<input->get("pusherEndHeight"); j++)
    {
        std::shared_ptr<PotentialPusher> myPusher = std::make_shared<PotentialPusher>(k, vD, lattice->leftNodes[j]->r().x(), tInit);
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

void SidePotentialLoading::dumpParameters()
{

    std::vector<string> outputParameters = {"nx","step","nt","grooveHeight","grooveSize","pusherStartHeight","pusherEndHeight"};
    for (string name: outputParameters){
        outfileParameters << name << " " << input->get(name) << "\n";
    }

    outfileParameters << "bottomNodes" << " " << lattice->bottomNodes.size() << "\n";

    outfileParameters.close();

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
