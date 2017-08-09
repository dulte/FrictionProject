// TODO: Buffer the IO operations
// TODO: Write tests. Things have a tendency to go wonky after a lot of refactoring
// TODO: Take a look at and improve vec3
#include <iostream>
#include "Simulation/simulation.h"

using namespace std;

int main(int argc, char *argv[])
{
    Simulation simulation;
    int retCode = simulation.setup();
    if (retCode != 0)
        return retCode;

    simulation.run();
    return 0;
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
