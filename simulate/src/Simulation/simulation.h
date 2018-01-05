#ifndef SIMULATION_H
#define SIMULATION_H

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include "FrictionSystem/frictionsystem.h"

class Parameters;

class Simulation
{
public:
    Simulation();
    virtual ~Simulation();
    int    setup();
    void   run();
    void   advanceProgress(int i);
    double timeSinceStart();
    void   startClock(){start = std::chrono::high_resolution_clock::now();};
    void   restartProgress(){progress = 0; prevProgress = 0;}
    void   releaseSprings();
    void   startDriving();
    void   releaseSpringsNstartDriving(){releaseSprings(); startDriving();}
    void   nop(){};

private:
    int    timestep = 0;
    double progress = 0;
    double prevProgress = 0;
    int    timeForNextPhase = 0;
    int    timeSinceLastPhase = 0;
    double step;
    std::string                                     parametersPath;
    std::shared_ptr<Parameters>                     parameters;
    std::shared_ptr<FrictionSystem>                 system;
    std::map<int, void (Simulation::*)()>           phases;
    std::chrono::high_resolution_clock::time_point  start;
    std::map<int, void (Simulation::*)()>::iterator nextPhase;
};


#endif /* SIMULATION_H */
