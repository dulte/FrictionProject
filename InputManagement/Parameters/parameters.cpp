#ifndef PARAMETERS_CPP
#define PARAMETERS_CPP

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <map>
#include <memory>
#include "parameters.h"
#include "parameter.h"

template <typename T>
std::unique_ptr<Parameter<T>> newParam(){
    return std::unique_ptr<Parameter<T>>(new Parameter<T>());
}

Parameters::Parameters(const std::string &filenameConfig){
    constructMap();
    readParameters(filenameConfig);
    checkThatAllParametersAreSet();
    m_infileParameters.close();
}

Parameters::~Parameters(){

}

template <>
void Parameters::addParameter<double>(std::string name){
    m_doubleparams[name] = newParam<double>();
}

template <>
void Parameters::addParameter<int>(std::string name){
    m_intparams[name] = newParam<int>();
}

template <>
void Parameters::addParameter<std::string>(std::string name){
    m_stringparams[name] = newParam<std::string>();
}

template <>
void Parameters::addParameter<bool>(std::string name){
    m_boolparams[name] = newParam<bool>();
}

template <>
int Parameters::get(std::string name){
    if (m_intparams.find(name) == m_intparams.end()){
        std::cerr << "Tried to get a non-existent int paramter: " << name << std::endl;
        throw std::exception();
    }
    return m_intparams[name]->get();
}

template <>
double Parameters::get(std::string name){
    if (m_doubleparams.find(name) == m_doubleparams.end()){
        std::cerr << "Tried to get a non-existent double paramter: " << name << std::endl;
        throw std::exception();
    }
    return m_doubleparams[name]->get();
}

template <>
std::string Parameters::get(std::string name){
    if (m_stringparams.find(name) == m_stringparams.end()){
        std::cerr << "Tried to get a non-existent string paramter: " << name << std::endl;
        throw std::exception();
    }
    return m_stringparams[name]->get();
}

template <>
bool Parameters::get(std::string name){
    if (m_boolparams.find(name) == m_boolparams.end()){
        std::cerr << "Tried to get a non-existent bool paramter: " << name << std::endl;
        throw std::exception();
    }
    return m_boolparams[name]->get();
}

void Parameters::constructMap(){
    addParameter<int>("nx");
    addParameter<int>("ny");
    addParameter<int>("nt");
    addParameter<int>("releaseTime");
    addParameter<int>("nt");
    addParameter<int>("releaseTime");
    addParameter<double>("fn");
    addParameter<double>("fn");
    addParameter<int>("ns");
    addParameter<double>("tau");
    addParameter<double>("d");
    addParameter<double>("E");
    addParameter<double>("k");
    addParameter<double>("nu");
    addParameter<double>("hZ");
    addParameter<double>("density");
    addParameter<double>("step");
    addParameter<double>("staticCoefficient");
    addParameter<std::string>("latticefilename");
    addParameter<int>("grooveSize");
    addParameter<int>("grooveHeight");
    addParameter<double>("vD");
    addParameter<double>("pK");
    addParameter<int>("pusherStartHeight");
    addParameter<int>("pusherEndHeight");
    addParameter<double>("beamMass");
    addParameter<double>("beamAngle");
    addParameter<int>("beamRotTime");
    addParameter<bool>("writeNodePositionInterface");
    addParameter<bool>("writeNodeVelocityInterface");
    addParameter<bool>("writeNodeSpringsAttachedInterface");
    addParameter<bool>("writeNodePositionAll");
    addParameter<bool>("writeNodeVelocityAll");
    addParameter<bool>("writeTotalEnergyAll");
    addParameter<bool>("writeNodeForceAll");
    addParameter<bool>("writePusherForce");
    addParameter<bool>("writeNormalForce");
    addParameter<bool>("writeShearForce");
    addParameter<bool>("writeBeamTorque");
    addParameter<bool>("writeBeamShearForce");
    addParameter<bool>("writeXYZ");
    addParameter<int>("freqNodePositionInterface");
    addParameter<int>("freqNodeVelocityInterface");
    addParameter<int>("freqNodeSpringsAttachedInterface");
    addParameter<int>("freqNodePositionAll");
    addParameter<int>("freqNodeVelocityAll");
    addParameter<int>("freqTotalEnergyAll");
    addParameter<int>("freqNodeForceAll");
    addParameter<int>("freqPusherForce");
    addParameter<int>("freqNormalForce");
    addParameter<int>("freqShearForce");
    addParameter<int>("freqBeamTorque");
    addParameter<int>("freqBeamShearForce");
    addParameter<int>("freqXYZ");
}


int Parameters::dumpParameters(){
    std::ofstream dumpFile(m_dumpFilename);
    if (!dumpFile.is_open()){
        std::cerr << "Could not dump parameters to file" << std::endl;
        return -1;
    }


    // dumpFile << "nx\t" << m_nx << "\n"
    //          << "ny\t" << m_ny << "\n"
    //          << "writingFreq\t" << m_writingFreq << "\n"
    //          << "nt\t" << m_nt << "\n"
    //          << "releaseTime\t" << m_releaseTime << "\n"
    //          << "fn\t" << m_fn << "\n"
    //          << "ns\t" << m_ns << "\n"
    //          << "tau\t" << m_tau << "\n"
    //          << "d\t" << m_d << "\n"
    //          << "E\t" << m_E << "\n"
    //          << "k\t" << m_k << "\n"
    //          << "nu\t" << m_nu << "\n"
    //          << "hZ\t" << m_hZ << "\n"
    //          << "density\t" << m_density << "\n"
    //          << "step\t" << m_step << "\n"
    //          << "latticeFilename\t" << m_latticeFilename << "\n"
    //          << "grooveSize\t" << m_grooveSize << "\n"
    //          << "grooveHeight\t" << m_grooveHeight << "\n"
    //          << "vD\t" << m_vD << "\n"
    //          << "pK\t" << m_pK << "\n"
    //          << "pusherStartHeight\t" << m_pusherStartHeight << "\n"
    //          << "pusherEndHeight\t" << m_pusherEndHeight << "\n"
    //          << "beamMass\t" << m_beamMass << "\n"
    //          << "beamAngle\t" << m_beamAngle << "\n"
    //          << "beamRotTime\t" << m_beamRotTime << "\n"
    //          << "writeNodePositionInterface\t" << m_writeNodePositionInterface << "\n"
    //          << "writeNodeVelocityInterface\t" << m_writeNodeVelocityInterface << "\n"
    //          << "writeNodeSpringsAttachedInterface\t" << m_writeNodeSpringsAttachedInterface << "\n"
    //          << "writeNodePositionAll\t" << m_writeNodePositionAll << "\n"
    //          << "writeNodeVelocityAll\t" << m_writeNodeVelocityAll << "\n"
    //          << "writeTotalEnergyAll\t" << m_writeTotalEnergyAll << "\n"
    //          << "writeNodeForceAll\t" << m_writeNodeForceAll << "\n"
    //          << "writePusherForce\t" << m_writePusherForce << "\n"
    //          << "writeNormalForce\t" << m_writeNormalForce << "\n"
    //          << "writeShearForce\t" << m_writeShearForce << "\n"
    //          << "writeXYZ\t" << m_writeXYZ << "\n"
    //          << "freqNodePositionInterface\t" << m_freqNodePositionInterface << "\n"
    //          << "freqNodeVelocityInterface\t" << m_freqNodeVelocityInterface << "\n"
    //          << "freqNodeSpringsAttachedInterface\t" << m_freqNodeSpringsAttachedInterface << "\n"
    //          << "freqNodePositionAll\t" << m_freqNodePositionAll << "\n"
    //          << "freqNodeVelocityAll\t" << m_freqNodeVelocityAll << "\n"
    //          << "freqTotalEnergyAll\t" << m_freqTotalEnergyAll << "\n"
    //          << "freqNodeForceAll\t" << m_freqNodeForceAll << "\n"
    //          << "freqPusherForce\t" << m_freqPusherForce << "\n"
    //          << "freqNormalForce\t" << m_freqNormalForce << "\n"
    //          << "freqShearForce\t" << m_freqShearForce << "\n"
    //          << "freqXYZ\t" << m_freqXYZ << std::endl;
    dumpFile.close();
    return 0;
}

void Parameters::readParameters(std::string filenameConfig){
    m_infileParameters.open(filenameConfig);

    if (!m_infileParameters) {
        std::cerr << "The config file " << filenameConfig << " could not be opened" << std::endl;
        throw std::runtime_error("The config could not be read");
    }

    std::string line;
    while (getline(m_infileParameters, line)) {
        // Cut the string into substrings
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        copy(std::istream_iterator<std::string>(iss),
             std::istream_iterator<std::string>(),
             back_inserter(tokens));

        // Parse the substrings
        if (tokens.size() == 0 || tokens[0][0] == '\n' || tokens[0][0] == '#')
          continue;
        if (m_intparams.find(tokens[0]) != m_intparams.end()) {
            m_intparams[tokens[0]]->read(tokens[1]);
        }
        else if (m_doubleparams.find(tokens[0]) != m_doubleparams.end()){
            m_doubleparams[tokens[0]]->read(tokens[1]);
        }
        else if (m_stringparams.find(tokens[0]) != m_stringparams.end()){
            m_stringparams[tokens[0]]->read(tokens[1]);
        }
        else if (m_boolparams.find(tokens[0]) != m_boolparams.end()){
            m_boolparams[tokens[0]]->read(tokens[1]);
        }
        else {
            std::cerr << "Unknown token in parameters: " << tokens[0] << std::endl;
            continue;
        }
    }
}

void Parameters::checkThatAllParametersAreSet(){
    for (auto& param: m_intparams){
        if (!param.second->isSet()){
            std::cerr << param.first << " is not set.";
            throw std::exception();
        }
    }
    for (auto& param: m_doubleparams){
        if (!param.second->isSet()){
            std::cerr << param.first << " is not set.";
            throw std::exception();
        }
    }
    for (auto& param: m_stringparams){
        if (!param.second->isSet()){
            std::cerr << param.first << " is not set.";
            throw std::exception();
        }
    }
    for (auto& param: m_boolparams){
        if (!param.second->isSet()){
            std::cerr << param.first << " is not set.";
            throw std::exception();
        }
    }
}
#endif /* PARAMETERS_CPP */
