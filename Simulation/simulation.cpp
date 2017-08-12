#include <iostream>
#include <memory>
#include <chrono>
#include <string>
#include <algorithm>
#include "simulation.h"
#include "InputManagement/Parameters/parameters.h"
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "FrictionSystem/TopPotentialLoading/toppotentialloading.h"
#include "FrictionSystem/Cantilever/cantilever.h"
#include "FrictionSystem/BulkWave/bulkwave.h"
#include "FrictionSystem/BulkStretch/bulkstretch.h"

Simulation::Simulation()
    :parametersPath("input/parameters.txt")
{}

Simulation::~Simulation() {}

int Simulation::setup() {
    // Get all of the configuration parameters
    try {
        std::cout << "Attempting to read parameters from " << parametersPath << std::endl;
        parameters = std::make_shared<Parameters>(parametersPath);
    } catch (std::exception &ex) {
        std::cerr << "Error: " <<ex.what() << std::endl;
        return -1;
    }

    step = parameters->get<double>("step");

    // Fill the time marks
    phases[parameters->get<int>("nt")] = &Simulation::nop;
    int releaseTime = parameters->get<int>("releaseTime");
    int drivingTime = parameters->get<int>("drivingTime");
    if (releaseTime == drivingTime){
        phases[releaseTime] = &Simulation::releaseSpringsNstartDriving;
    } else {
        phases[parameters->get<int>("releaseTime")] = &Simulation::releaseSprings;
        phases[parameters->get<int>("drivingTime")] = &Simulation::startDriving;
    }

    // std::sort(phases.begin(), phases.end());

    // Set the time mark to the first mark
    nextPhase = phases.begin();
    timeForNextPhase = (*nextPhase).first;

    try {
        std::cout << "Constructing system" << std::endl;
        system = std::make_shared<BulkStretch>(parameters);
    } catch (std::exception &ex) {
        std::cerr << "Error> " << ex.what() << std::endl;
    }
    return 0;
}


void Simulation::run() {
    startClock();
    system->isLockFrictionSprings(true);
    std::cout << "Starting the simulation with the model stationary and springs locked at " << timeSinceStart() << std::endl;
    while (true){
        system->step(step, timestep);
        timestep++;
        advanceProgress(timestep);

        if (timestep > timeForNextPhase){
            // Run the corresponding function
            // using the ugliest notation in the C++-language
            (this->*((*nextPhase).second))();
            ++nextPhase;
            if(nextPhase == phases.end())
                break;

            restartProgress();
            timeSinceLastPhase = timeForNextPhase;
            timeForNextPhase   = (*nextPhase).first;
        }
    }
    std::cout << "Simulation complete at " << timeSinceStart() << std::endl;
    system->postProcessing();
}

void Simulation::releaseSprings(){
    std::cout << "Releasing springs at " << timeSinceStart() << std::endl;
    system->isLockFrictionSprings(false);
}

void Simulation::startDriving(){
    std::cout << "Starting to drive at " << timeSinceStart() << std::endl;
    system->startDriving(timestep*step);
}

void Simulation::advanceProgress(int i){
    if (progress >= prevProgress){
        std::cout << 100*progress << "% completed" << std::endl;
        prevProgress += 0.1;
    }
    progress = static_cast<double>(i-timeSinceLastPhase)/(timeForNextPhase-timeSinceLastPhase);
}

double Simulation::timeSinceStart(){
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto time = std::chrono::duration_cast<std::chrono::duration<double>>(diff).count();
    return time;
}
