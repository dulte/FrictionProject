#include "sidepotentialloading.h"
#include <iostream>


#include <ForceModifier/ConstantForce/constantforce.h>
#include <ForceModifier/PotentialSurface/potentialsurface.h>
#include <ForceModifier/PotentialPusher/potentialpusher.h>
#include <ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h>
#include <ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h>
#include "ForceModifier/SpringFriction/springfriction.h"
#include "FrictionInfo/frictioninfo.h"

SidePotentialLoading::SidePotentialLoading(int nx, int ny, double d, double E, double k, double topLoadingForce)
{
    std::string outFileFolder = "/Output";
    outfile.open(outFileFolder+std::string("testnumsprings.bin"));
    outfileNormalForces.open(outFileFolder+std::string("normalforces.bin"));
    outfilePusherForces.open(outFileFolder+std::string("pusherforces.bin"));

    lattice = std::make_unique<TriangularLattice>();
    lattice->populate(nx, ny, d, E, 0.33, 0.006, 1300);

    std::shared_ptr<FrictionInfo> frictionInfo = std::make_shared<FrictionInfo>();

    int numTop = lattice->topNodes.size();
    for (int i = 0; i<numTop; i++)
    {
        std::unique_ptr<ConstantForce> myForce = std::make_unique<ConstantForce>(vec3(0, -topLoadingForce/numTop, 0));
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
    outfile.close();
}

void SidePotentialLoading::addPusher(double k, double vD, double tInit)
{
    for (int j = 2; j<3; j++)
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

void SidePotentialLoading::dumpData()
{
    for (auto & frictionElement : frictionElements)
    {
        double numSprings = frictionElement->m_numSpringsAttached;
//        std::cout << numSprings << std::endl;
        outfile.write((char*)&numSprings, sizeof(double));
        outfileNormalForces.write((char*)&frictionElement->m_normalForce, sizeof(double));

    }
    double pushForce = 0;
    for (auto & pusherNode : pusherNodes)
    {
        pushForce += pusherNode->fPush;
    }
    outfilePusherForces.write((char*)&pushForce, sizeof(double));
}

std::vector<DataPacket> SidePotentialLoading::getDataPackets(int timestep, double time)
{
    std::vector<DataPacket> packets = lattice->getDataPackets(timestep, time);
    return packets;
}
