#ifndef SIMULATION_H
#define SIMULATION_H

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"

class Parameters;

class Simulation
{
public:
    Simulation();
    virtual ~Simulation();
    int setup();
    void run();
    void advanceProgress(int i);
    double timeSinceStart();
    void startClock(){start = std::chrono::high_resolution_clock::now();};
    void restartProgress(){progress = 0; prevProgress = 0;}
    void releaseSprings();
    void startDriving();
    void nop(){};

private:
    int timestep = 0;
    double progress = 0;
    double prevProgress = 0;
    int timeForNextPhase = 0;
    int timeSinceLastPhase = 0;
    std::chrono::high_resolution_clock::time_point start;
    std::string parametersPath;
    double step;
    std::shared_ptr<SidePotentialLoading> system;
    std::shared_ptr<Parameters> parameters;
    std::map<int, void (Simulation::*)()> phases;
    std::map<int, void (Simulation::*)()>::iterator nextPhase;
};


#endif /* SIMULATION_H */
