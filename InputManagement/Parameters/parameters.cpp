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
std::unique_ptr<ParameterInterface> newParam(){
    return std::unique_ptr<ParameterInterface>(new Parameter<T>());
}

Parameters::Parameters(const std::string &filenameConfig){
    constructMap();
    readParameters(filenameConfig);
    checkThatAllParametersAreSet();
    m_infileParameters.close();
}

Parameters::~Parameters(){

}

void Parameters::constructMap(){
      m_parameters["nx"]                                = newParam<int>();
      m_parameters["ny"]                                = newParam<int>();
      m_parameters["nt"]                                = newParam<int>();
      m_parameters["releaseTime"]                       = newParam<int>();
      m_parameters["nt"]                                = newParam<int>();
      m_parameters["releaseTime"]                       = newParam<int>();
      m_parameters["fn"]                                = newParam<double>();
      m_parameters["fn"]                                = newParam<double>();
      m_parameters["ns"]                                = newParam<double>();
      m_parameters["tau"]                               = newParam<double>();
      m_parameters["d"]                                 = newParam<double>();
      m_parameters["E"]                                 = newParam<double>();
      m_parameters["k"]                                 = newParam<double>();
      m_parameters["nu"]                                = newParam<double>();
      m_parameters["hZ"]                                = newParam<double>();
      m_parameters["density"]                           = newParam<double>();
      m_parameters["step"]                              = newParam<double>();
      m_parameters["staticCoefficient"]                 = newParam<double>();
      m_parameters["latticefilename"]                   = newParam<std::string>();
      m_parameters["grooveSize"]                        = newParam<int>();
      m_parameters["grooveHeight"]                      = newParam<int>();
      m_parameters["vD"]                                = newParam<double>();
      m_parameters["pK"]                                = newParam<double>();
      m_parameters["pusherStartHeight"]                 = newParam<int>();
      m_parameters["pusherEndHeight"]                   = newParam<int>();
      m_parameters["beamMass"]                          = newParam<double>();
      m_parameters["beamAngle"]                         = newParam<double>();
      m_parameters["beamRotTime"]                       = newParam<int>();
      m_parameters["writeNodePositionInterface"]        = newParam<bool>();
      m_parameters["writeNodeVelocityInterface"]        = newParam<bool>();
      m_parameters["writeNodeSpringsAttachedInterface"] = newParam<bool>();
      m_parameters["writeNodePositionAll"]              = newParam<bool>();
      m_parameters["writeNodeVelocityAll"]              = newParam<bool>();
      m_parameters["writeTotalEnergyAll"]               = newParam<bool>();
      m_parameters["writeNodeForceAll"]                 = newParam<bool>();
      m_parameters["writePusherForce"]                  = newParam<bool>();
      m_parameters["writeNormalForce"]                  = newParam<bool>();
      m_parameters["writeShearForce"]                   = newParam<bool>();
      m_parameters["writeBeamTorque"]                   = newParam<bool>();
      m_parameters["writeBeamShearForce"]               = newParam<bool>();
      m_parameters["writeXYZ"]                          = newParam<bool>();
      m_parameters["freqNodePositionInterface"]         = newParam<int>();
      m_parameters["freqNodeVelocityInterface"]         = newParam<int>();
      m_parameters["freqNodeSpringsAttachedInterface"]  = newParam<int>();
      m_parameters["freqNodePositionAll"]               = newParam<int>();
      m_parameters["freqNodeVelocityAll"]               = newParam<int>();
      m_parameters["freqTotalEnergyAll"]                = newParam<int>();
      m_parameters["freqNodeForceAll"]                  = newParam<int>();
      m_parameters["freqPusherForce"]                   = newParam<int>();
      m_parameters["freqNormalForce"]                   = newParam<int>();
      m_parameters["freqShearForce"]                    = newParam<int>();
      m_parameters["freqBeamTorque"]                    = newParam<int>();
      m_parameters["freqBeamShearForce"]                = newParam<int>();
      m_parameters["freqXYZ"]                           = newParam<int>();
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
        if (m_parameters.find(tokens[0]) == m_parameters.end()){
            std::cerr << "Unknown token in parameters: " << tokens[0] << std::endl;
            continue;
        }
        m_parameters[tokens[0]]->read(tokens[1]);
    }
}

void Parameters::checkThatAllParametersAreSet(){
    for (auto& param: m_parameters){
        if (!param.second->isSet()){
            std::cerr << param.first << " is not set.";
            throw std::exception();
        }
    }
}
#endif /* PARAMETERS_CPP */
