// TODO: Buffer the IO operations
// TODO: Write tests. Things have a tendency to go wonky after a lot of refactoring
// TODO: Take a look at and improve vec3
// TODO: Shear force på topblokkene
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <time.h>
#include <omp.h>
#include "InputManagement/Parameters/parameter.h"
// #include <boost/filesystem.hpp>
// #include <boost/program_options.hpp>
// #include "ForceModifier/ConstantForce/constantforce.h"
// #include "ForceModifier/PotentialSurface/potentialsurface.h"
// #include "ForceModifier/ConstantMoment/constantmoment.h"
// #include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"
// #include "ForceModifier/AbsoluteOmegaDamper/absoluteomegadamper.h"
// #include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "DataOutput/datapackethandler.h"
#include "InputManagement/Parameters/parameters.h"

using namespace std;

// void cantileverTest(TriangularLattice &);
// void bulkWave(TriangularLattice &);
// void bulkStretch(TriangularLattice &);
double timeSince(const clock_t &);

int main(int argc, char *argv[])
{
    std::string outputDirectory = "output";
    std::string parametersPath = "input/parameters.txt";
    bool doDumpParameters = false;
    // Parse and handle commandline arguments
    // This works only partially. Fix
    // try{
    //     boost::program_options::options_description desc{"Options"};
    //     desc.add_options()
    //         ("help,h", "Help screen")
    //         ("output,o", boost::program_options::value<std::string>()->default_value("output/"), "outputDirectory")
    //         ("parameters,p", boost::program_options::value<std::string>()->default_value("input/parameters.txt"), "config")
    //         ("dumpparams,d", boost::program_options::value<bool>()->default_value(false), "doDumpParameters");
    //     boost::program_options::variables_map vm;
    //     boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    //     boost::program_options::notify(vm);
    //     doDumpParameters = vm["dumpparams"].as<bool>();
    //     outputDirectory  = vm["output"].as<std::string>();
    //     parametersPath   = vm["parameters"].as<std::string>();

    //     bool isOutputCreated = boost::filesystem::create_directories(outputDirectory);
    //     if (!isOutputCreated){
    //         if (boost::filesystem::exists(outputDirectory))
    //             std::cout << "Output directory exists. Overwriting files within." << std::endl;
    //         else {
    //             std::cerr << "Could not make output directory" << std::endl;
    //             return -1;
    //         }
    //     }
    // } catch (const std::exception &ex) {
    //     std::cerr << "Error: " << ex.what() << std::endl;
    //     return -1;
    // }


    clock_t     start = clock();
    // Get all of the configuration parameters
    std::shared_ptr<Parameters> spParameters;
    try {
        std::cout << "Reading configuration parameters at " << timeSince(start) << std::endl;
        spParameters = std::make_shared<Parameters>(parametersPath);
    } catch (std::exception &ex) {
        std::cerr << "Error: " <<ex.what() << std::endl;
        return -1;
    }

    if (doDumpParameters){
        std::cout << "Dumping parameters to " << spParameters->get<std::string>("dumpfilename") << std::endl;
        spParameters->dumpParameters();
    }

    int    nt          = spParameters->get<int>("nt");
    int    releaseTime = spParameters->get<int>("releaseTime");
    double step        = spParameters->get<double>("step");

    // For the progress output
    double progress     = 0;
    double prevProgress = 0;

    SidePotentialLoading mySystem(spParameters);
    DataPacketHandler    dataPacketHandler(outputDirectory, spParameters);

    mySystem.isLockFrictionSprings(true);
    std :: cout << "Starting the model with springs locked at " << timeSince(start) << std::endl;
    for (int i = 0; i<releaseTime; i++)
    {
        mySystem.step(step);
        mySystem.m_driverBeam->checkRotation();
        dataPacketHandler.dumpXYZ(mySystem, i);
        dataPacketHandler.step(mySystem.getDataPackets(i, i*step));

        if (progress >= prevProgress){
            std::cout << 100*progress << "% completed" << std::endl;
            prevProgress += 0.1;
        }
        progress = static_cast<double>(i)/releaseTime;
    }

    // TODO: Why call mySystem.lattice->t()? Can't the system do it internally?
    mySystem.startDriving();
    mySystem.isLockFrictionSprings(false);
    std::cout << "Unlocking springs at " << timeSince(start) << std::endl;
    prevProgress = 0;
    progress = 0;
    for (int i = releaseTime; i<nt; i++)
    {
        mySystem.step(step);
        dataPacketHandler.dumpXYZ(mySystem, i);
        dataPacketHandler.step(mySystem.getDataPackets(i, i*step));

        if (progress >= prevProgress){
            std::cout << 100*progress << "% completed" << std::endl;
            prevProgress += 0.1;
        }
        progress = static_cast<double>(i-releaseTime)/(nt-releaseTime);
    }

    std::cout << "Simulation complete at " << timeSince(start) << std::endl;

    return 0;
}

double timeSince(const clock_t &start){
   clock_t now = clock();
   return static_cast<double>(now-start)/CLOCKS_PER_SEC;
}


// void cantileverTest(TriangularLattice & lattice)
// {
//     lattice.populateCantilever(0.005, 3e9, 0.33, 0.006, 1300);
//     std::unique_ptr<ConstantForce> myForce = std::make_unique<ConstantForce>(vec3(0, -200, 0));
//     lattice.nodes[1]->addModifier(std::move(myForce));
//     std::unique_ptr<RelativeVelocityDamper> myModifier = std::make_unique<RelativeVelocityDamper>(10);
//     lattice.nodes[1]->addModifier(std::move(myModifier));
//     lattice.nodes[0]->isSetForce(true);
// }

// void bulkWave(TriangularLattice & lattice)
// {
//     int nx = 41;
//     int ny = 21;
//     double d = 0.005;
//     lattice.populate(nx, ny, d, 3e9, 0.33, 0.006, 1300);
//     for (int i = 0; i<ny; i++)
//     {
//         lattice.nodes[i*nx]->pertubatePosition(vec3(d*0.1, 0, 0));
//         lattice.nodes[i*nx+1]->pertubatePosition(vec3(d*0.05, 0, 0));
//     }
//     for (auto & node : lattice.nodes)
//     {
//         std::unique_ptr<RelativeVelocityDamper> myModifier = std::make_unique<RelativeVelocityDamper>(0.1);
//         node->addModifier(std::move(myModifier));
//     }
// }

// void bulkStretch(TriangularLattice & lattice)
// {
//     int nx = 21;
//     int ny = 21;
//     double d = 0.005;
//     lattice.populate(nx, ny, d, 3e9, 0.33, 0.006, 1300);
//     for (int i = 0; i<ny; i++)
//     {
//         std::unique_ptr<ConstantForce> myModifier = std::make_unique<ConstantForce>(vec3(-10000, 0, 0));
//         lattice.nodes[nx*i]->addModifier(std::move(myModifier));
//         std::unique_ptr<ConstantForce> myModifier2 = std::make_unique<ConstantForce>(vec3(10000, 0, 0));
//         lattice.nodes[nx*i+nx-1]->addModifier(std::move(myModifier2));
//     }
//     for (auto & node : lattice.nodes)
//     {
//         std::unique_ptr<RelativeVelocityDamper> myDamper = std::make_unique<RelativeVelocityDamper>(0.1);
//         node->addModifier(std::move(myDamper));
//     }

// }
