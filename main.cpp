#include <iostream>
#include <string>
#include <Lattice/TriangularLattice/triangularlattice.h>
#include "Lattice/SquareLattice/squarelattice.h"
//#include <QDebug>
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
#include "InputManagement/ConfigReader/configreader.h"


using namespace std;

void cantileverTest(TriangularLattice &);
void bulkWave(TriangularLattice &);
void bulkStretch(TriangularLattice &);

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
    std::ofstream myStream("out.xyz",std::ofstream::out);
    ConfigReader *input = new ConfigReader("config.txt");

    int nx = int(input->get("nx"));
    int ny = int(input->get("ny"));
    int writingFreq = int(input->get("writingFreq"));

    SidePotentialLoading mySystem(nx, ny, 0.005, 3e9, 4e6, input->get("fn"));
    DataPacketHandler dataPacketHandler(outputFolder, input);
    int nt = int(input->get("nt"));
    int releaseTime = input->get("releaseTime");
    double step = (input->get("step"));
    mySystem.dumpParameters();


    mySystem.isLockFrictionSprings(true);
    for (int i = 0; i<releaseTime; i++)
    {
        mySystem.lattice->step(step);
        if (i%writingFreq == 0)
        {
            std::cout << static_cast<double>(i)/nt << std::endl;
            dataPacketHandler.step(mySystem.getDataPackets(i, i*step));
            dataPacketHandler.dumpXYZ(mySystem.lattice->xyzString());
        }
    }
    mySystem.addPusher(4e6, 4e-4, mySystem.lattice->t());
    mySystem.isLockFrictionSprings(false);
    //nt = 2000;
    for (int i = releaseTime; i<nt; i++)
    {
        mySystem.lattice->step(step);
        if (i%writingFreq == 0)
        {
            std::cout << static_cast<double>(i)/nt << std::endl;
            dataPacketHandler.step(mySystem.getDataPackets(i, i*step));
            dataPacketHandler.dumpXYZ(mySystem.lattice->xyzString());
        }
    }

    clock_t stop = clock();
    cout << static_cast<double>(stop-start)/CLOCKS_PER_SEC << std::endl;
    return 0;
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
