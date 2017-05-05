// TODO: Write a function to dump all relevant parameters to a file
#include <iostream>
#include <string>
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include <fstream>
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/PotentialSurface/potentialsurface.h"
#include "ForceModifier/ConstantMoment/constantmoment.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
#include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "DataOutput/datapackethandler.h"
#include <memory>
#include <time.h>
#include <omp.h>
#include "InputManagement/Parameters/parameters.h"

using namespace std;

void cantileverTest(TriangularLattice &);
void bulkWave(TriangularLattice &);
void bulkStretch(TriangularLattice &);
double getTime(const clock_t &);

int main(int argc, char *argv[])
{

    clock_t start = clock();
    std::string outputFolder;

    if (argc > 1){
        outputFolder = argv[1];
    }
    else{
        outputFolder = "Output/";
    }

    // Get all of the configuration parameters
    std::cout << "Reading configuration parameters at " << getTime(start) << std::endl;
    std::shared_ptr<Parameters> spParameters = std::make_shared<Parameters>("Config/config.txt");

    int    nt          = spParameters->m_nt;
    int    releaseTime = spParameters->m_releaseTime;
    double step        = spParameters->m_step;
    int    writingFreq = nt/10;

    SidePotentialLoading mySystem(spParameters);
    DataPacketHandler    dataPacketHandler(outputFolder, spParameters);

    std::cout<<releaseTime<<std::endl;
    mySystem.isLockFrictionSprings(true);
    std :: cout << "Starting the model with springs locked at " << getTime(start) << std::endl;
    for (int i = 0; i<releaseTime; i++)
    {
        mySystem.lattice->step(step);
        if (writingFreq != 0 && i%writingFreq == 0)
            std::cout << static_cast<double>(i)/nt << std::endl;
        dataPacketHandler.dumpXYZ(mySystem.lattice, i);
        dataPacketHandler.step(mySystem.getDataPackets(i, i*step));
    }

    mySystem.addPusher(mySystem.lattice->t());
    mySystem.isLockFrictionSprings(false);
    std::cout << "Unlocking springs at " << getTime(start) << std::endl;
    //nt = 2000;
    for (int i = releaseTime; i<nt; i++)
    {
        mySystem.lattice->step(step);
        if (i%writingFreq == 0)
            std::cout << static_cast<double>(i)/nt << std::endl;

        dataPacketHandler.dumpXYZ(mySystem.lattice, i);
        dataPacketHandler.step(mySystem.getDataPackets(i, i*step));
    }

    std::cout << "Simulation complete at " << getTime(start) << std::endl;

    return 0;
}

double getTime(const clock_t &start){
   clock_t now = clock();
   return static_cast<double>(now-start)/CLOCKS_PER_SEC;
}

void cantileverTest(TriangularLattice & lattice)
{
    lattice.populateCantilever(0.005, 3e9, 0.33, 0.006, 1300);
    std::unique_ptr<ConstantForce> myForce = std::make_unique<ConstantForce>(vec3(0, -200, 0));
    lattice.nodes[1]->addModifier(std::move(myForce));
    std::unique_ptr<RelativeVelocityDamper> myModifier = std::make_unique<RelativeVelocityDamper>(10);
    lattice.nodes[1]->addModifier(std::move(myModifier));
    lattice.nodes[0]->isSetForce(true);
}

void bulkWave(TriangularLattice & lattice)
{
    int nx = 41;
    int ny = 21;
    double d = 0.005;
    lattice.populate(nx, ny, d, 3e9, 0.33, 0.006, 1300);
    for (int i = 0; i<ny; i++)
    {
        lattice.nodes[i*nx]->pertubatePosition(vec3(d*0.1, 0, 0));
        lattice.nodes[i*nx+1]->pertubatePosition(vec3(d*0.05, 0, 0));
    }
    for (auto & node : lattice.nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> myModifier = std::make_unique<RelativeVelocityDamper>(0.1);
        node->addModifier(std::move(myModifier));
    }
}

void bulkStretch(TriangularLattice & lattice)
{
    int nx = 21;
    int ny = 21;
    double d = 0.005;
    lattice.populate(nx, ny, d, 3e9, 0.33, 0.006, 1300);
    for (int i = 0; i<ny; i++)
    {
        std::unique_ptr<ConstantForce> myModifier = std::make_unique<ConstantForce>(vec3(-10000, 0, 0));
        lattice.nodes[nx*i]->addModifier(std::move(myModifier));
        std::unique_ptr<ConstantForce> myModifier2 = std::make_unique<ConstantForce>(vec3(10000, 0, 0));
        lattice.nodes[nx*i+nx-1]->addModifier(std::move(myModifier2));
    }
    for (auto & node : lattice.nodes)
    {
        std::unique_ptr<RelativeVelocityDamper> myDamper = std::make_unique<RelativeVelocityDamper>(0.1);
        node->addModifier(std::move(myDamper));
    }

}
