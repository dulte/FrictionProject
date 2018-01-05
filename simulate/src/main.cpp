// TODO: Take a look at and improve vec3
// TODO: Start hastigheten sakt
#include "Simulation/simulation.h"

int main(int argc, char *argv[])
{
    Simulation simulation;
    int retCode = simulation.setup();
    if (retCode != 0)
        return retCode;

    simulation.run();
    return 0;
}
