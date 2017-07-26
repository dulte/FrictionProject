#ifndef PARAMETERS_CPP
#define PARAMETERS_CPP

#include "parameters.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>

Parameters::Parameters(const std::string &filenameConfig){
    readParameters(filenameConfig);
    checkThatAllParametersAreSet();
    m_infileParameters.close();
}

Parameters::~Parameters(){
}

int Parameters::dumpParameters(){
    std::ofstream dumpFile(m_dumpFilename);
    if (!dumpFile.is_open()){
        std::cerr << "Could not dump parameters to file" << std::endl;
        return -1;
    }


    dumpFile << "nx\t" << m_nx << "\n"
             << "ny\t" << m_ny << "\n"
             << "writingFreq\t" << m_writingFreq << "\n"
             << "nt\t" << m_nt << "\n"
             << "releaseTime\t" << m_releaseTime << "\n"
             << "fn\t" << m_fn << "\n"
             << "ns\t" << m_ns << "\n"
             << "tau\t" << m_tau << "\n"
             << "d\t" << m_d << "\n"
             << "E\t" << m_E << "\n"
             << "k\t" << m_k << "\n"
             << "nu\t" << m_nu << "\n"
             << "hZ\t" << m_hZ << "\n"
             << "density\t" << m_density << "\n"
             << "step\t" << m_step << "\n"
             << "latticeFilename\t" << m_latticeFilename << "\n"
             << "grooveSize\t" << m_grooveSize << "\n"
             << "grooveHeight\t" << m_grooveHeight << "\n"
             << "vD\t" << m_vD << "\n"
             << "pK\t" << m_pK << "\n"
             << "pusherStartHeight\t" << m_pusherStartHeight << "\n"
             << "pusherEndHeight\t" << m_pusherEndHeight << "\n"
             << "beamMass\t" << m_beamMass << "\n"
             << "beamAngle\t" << m_beamAngle << "\n"
             << "beamRotTime\t" << m_beamRotTime << "\n"
             << "writeNodePositionInterface\t" << m_writeNodePositionInterface << "\n"
             << "writeNodeVelocityInterface\t" << m_writeNodeVelocityInterface << "\n"
             << "writeNodeSpringsAttachedInterface\t" << m_writeNodeSpringsAttachedInterface << "\n"
             << "writeNodePositionAll\t" << m_writeNodePositionAll << "\n"
             << "writeNodeVelocityAll\t" << m_writeNodeVelocityAll << "\n"
             << "writeTotalEnergyAll\t" << m_writeTotalEnergyAll << "\n"
             << "writeTotalForceAll\t" << m_writeTotalForceAll << "\n"
             << "writePusherForce\t" << m_writePusherForce << "\n"
             << "writeNormalForce\t" << m_writeNormalForce << "\n"
             << "writeShearForce\t" << m_writeShearForce << "\n"
             << "writeXYZ\t" << m_writeXYZ << "\n"
             << "freqNodePositionInterface\t" << m_freqNodePositionInterface << "\n"
             << "freqNodeVelocityInterface\t" << m_freqNodeVelocityInterface << "\n"
             << "freqNodeSpringsAttachedInterface\t" << m_freqNodeSpringsAttachedInterface << "\n"
             << "freqNodePositionAll\t" << m_freqNodePositionAll << "\n"
             << "freqNodeVelocityAll\t" << m_freqNodeVelocityAll << "\n"
             << "freqTotalEnergyAll\t" << m_freqTotalEnergyAll << "\n"
             << "freqTotalForceAll\t" << m_freqTotalForceAll << "\n"
             << "freqPusherForce\t" << m_freqPusherForce << "\n"
             << "freqNormalForce\t" << m_freqNormalForce << "\n"
             << "freqShearForce\t" << m_freqShearForce << "\n"
             << "freqXYZ\t" << m_freqXYZ << std::endl;
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
        if (tokens.size() == 0) {
          // Empty line, continue
          continue;
        } else if (tokens[0] == "nx") {
            m_nx = std::stoi(tokens[1]);
            m_bnx = 1;
        } else if(tokens[0] == "staticCoefficient"){
            m_staticCoefficient = std::stod(tokens[1]);
            m_bstaticCoefficient = 1;
        } else if (tokens[0] == "ny") {
            m_ny = std::stoi(tokens[1]);
            m_bny = 1;
        } else  if (tokens[0] == "writingFreq") {
            // Integers written in e-notation need to be read as double and converted to int
            m_writingFreq = static_cast<int>(std::stod(tokens[1]));
            m_bwritingFreq = 1;
        } else  if (tokens[0] == "nt") {
            m_nt = std::stod(tokens[1]);
            m_bnt = 1;
        } else  if (tokens[0] == "releaseTime") {
            m_releaseTime = static_cast<int>(std::stod(tokens[1]));
            m_breleaseTime = 1;
        } else  if (tokens[0] == "fn") {
            m_fn = std::stod(tokens[1]);
            m_bfn = 1;
        } else  if (tokens[0] == "ns") {
            m_ns = std::stod(tokens[1]);
            m_bns = 1;
        } else  if (tokens[0] == "tau") {
            m_tau = std::stod(tokens[1]);
            m_btau = 1;
        } else  if (tokens[0] == "d") {
            m_d = std::stod(tokens[1]);
            m_bd = 1;
        } else  if (tokens[0] == "E") {
            m_E = std::stod(tokens[1]);
            m_bE = 1;
        } else  if (tokens[0] == "k") {
            m_k = std::stod(tokens[1]);
            m_bk = 1;
        } else  if (tokens[0] == "nu") {
            m_nu = std::stod(tokens[1]);
            m_bnu = 1;
        } else  if (tokens[0] == "hZ") {
            m_hZ = std::stod(tokens[1]);
            m_bhZ = 1;
        } else  if (tokens[0] == "density") {
            m_density = std::stod(tokens[1]);
            m_bdensity = 1;
        } else  if (tokens[0] == "step") {
            m_step = std::stod(tokens[1]);
            m_bstep = 1;
        } else  if (tokens[0] == "grooveSize") {
            m_grooveSize = std::stoi(tokens[1]);
            m_bgrooveSize = 1;
        } else  if (tokens[0] == "grooveHeight") {
            m_grooveHeight = std::stoi(tokens[1]);
            m_bgrooveHeight = 1;
        } else  if (tokens[0] == "vD") {
            m_vD = std::stod(tokens[1]);
            m_bvD = 1;
        } else  if (tokens[0] == "pK") {
            m_pK = std::stod(tokens[1]);
            m_bpK = 1;
        } else  if (tokens[0] == "pusherStartHeight") {
            m_pusherStartHeight = std::stoi(tokens[1]);
            m_bpusherStartHeight = 1;
        } else  if (tokens[0] == "pusherEndHeight") {
            m_pusherEndHeight = std::stoi(tokens[1]);
            m_bpusherEndHeight = 1;
        } else  if (tokens[0] == "writeNodePositionInterface") {
            m_writeNodePositionInterface = std::stoi(tokens[1]);
            m_bwriteNodePositionInterface = 1;
        } else  if (tokens[0] == "writeNodeVelocityInterface") {
            m_writeNodeVelocityInterface = std::stoi(tokens[1]);
            m_bwriteNodeVelocityInterface = 1;
        } else  if (tokens[0] == "writeNodeSpringsAttachedInterface") {
            m_writeNodeSpringsAttachedInterface = std::stoi(tokens[1]);
            m_bwriteNodeSpringsAttachedInterface = 1;
        } else  if (tokens[0] == "writeNodePositionAll") {
            m_writeNodePositionAll = std::stoi(tokens[1]);
            m_bwriteNodePositionAll = 1;
        } else  if (tokens[0] == "writeNodeVelocityAll") {
            m_writeNodeVelocityAll = std::stoi(tokens[1]);
            m_bwriteNodeVelocityAll = 1;
        } else  if (tokens[0] == "writeTotalEnergyAll") {
            m_writeTotalEnergyAll = std::stoi(tokens[1]);
            m_bwriteTotalEnergyAll = 1;
        } else  if (tokens[0] == "writeTotalForceAll") {
            m_writeTotalForceAll = std::stoi(tokens[1]);
            m_bwriteTotalForceAll = 1;
        } else  if (tokens[0] == "writePusherForce") {
            m_writePusherForce = std::stoi(tokens[1]);
            m_bwritePusherForce = 1;
        } else  if (tokens[0] == "writeNormalForce") {
            m_writeNormalForce = std::stoi(tokens[1]);
            m_bwriteNormalForce = 1;
        } else  if (tokens[0] == "writeShearForce") {
            m_writeShearForce = std::stoi(tokens[1]);
            m_bwriteShearForce = 1;
        } else  if (tokens[0] == "writeXYZ") {
            m_writeXYZ = std::stoi(tokens[1]);
            m_bwriteXYZ = 1;
        } else  if (tokens[0] == "writeBeamShearForce") {
            m_writeBeamShearForce = std::stoi(tokens[1]);
            m_bwriteBeamShearForce = 1;
        } else  if (tokens[0] == "writeBeamTorque") {
            m_writeBeamTorque = std::stoi(tokens[1]);
            m_bwriteBeamTorque = 1;
        } else  if (tokens[0] == "freqNodePositionInterface") {
            m_freqNodePositionInterface = std::stoi(tokens[1]);
            m_bfreqNodePositionInterface = 1;
        } else  if (tokens[0] == "freqNodeVelocityInterface") {
            m_freqNodeVelocityInterface = std::stoi(tokens[1]);
            m_bfreqNodeVelocityInterface = 1;
        } else  if (tokens[0] == "freqNodeSpringsAttachedInterface") {
            m_freqNodeSpringsAttachedInterface = std::stoi(tokens[1]);
            m_bfreqNodeSpringsAttachedInterface = 1;
        } else  if (tokens[0] == "freqNodePositionAll") {
            m_freqNodePositionAll = std::stoi(tokens[1]);
            m_bfreqNodePositionAll = 1;
        } else  if (tokens[0] == "freqNodeVelocityAll") {
            m_freqNodeVelocityAll = std::stoi(tokens[1]);
            m_bfreqNodeVelocityAll = 1;
        } else  if (tokens[0] == "freqTotalEnergyAll") {
            m_freqTotalEnergyAll = std::stoi(tokens[1]);
            m_bfreqTotalEnergyAll = 1;
        } else  if (tokens[0] == "freqTotalForceAll") {
            m_freqTotalForceAll = std::stoi(tokens[1]);
            m_bfreqTotalForceAll = 1;
        } else  if (tokens[0] == "freqPusherForce") {
            m_freqPusherForce = std::stoi(tokens[1]);
            m_bfreqPusherForce = 1;
        } else  if (tokens[0] == "freqNormalForce") {
            m_freqNormalForce = std::stoi(tokens[1]);
            m_bfreqNormalForce = 1;
        } else  if (tokens[0] == "freqShearForce") {
            m_freqShearForce = std::stoi(tokens[1]);
            m_bfreqShearForce = 1;
        } else  if (tokens[0] == "freqXYZ") {
            m_freqXYZ = std::stoi(tokens[1]);
            m_bfreqXYZ = 1;
        } else  if (tokens[0] == "freqBeamShearForce") {
            m_freqBeamShearForce = std::stoi(tokens[1]);
            m_bfreqBeamShearForce = 1;
        } else  if (tokens[0] == "freqBeamTorque") {
            m_freqBeamTorque = std::stoi(tokens[1]);
            m_bfreqBeamTorque = 1;
        } else if (tokens[0] == "latticefilename") {
            m_latticeFilename = tokens[1];
            m_blatticeFilename = 1;
        } else if (tokens[0] == "beamMass") {
            m_beamMass = std::stod(tokens[1]);
            m_bbeamMass = 1;
        } else if (tokens[0] == "beamAngle") {
            m_beamAngle = std::stod(tokens[1]);
            m_bbeamAngle = 1;
        } else if (tokens[0] == "beamRotTime") {
            m_beamRotTime = static_cast<int>(std::stod(tokens[1]));
            m_bbeamRotTime = 1;
        } else if (tokens[0][0] == '\n' || tokens[0][0] == '#') {
            continue;
        } else {
            std::cerr << "Unknown token in config file: " << tokens[0] << std::endl;
        }
    }
}

void Parameters::checkThatAllParametersAreSet(){
    if (!m_bnx) {
        throw std::runtime_error("The parameter nx is not set");
    } else if (!m_bny) {
        throw std::runtime_error("The parameter ny is not set");
    } else if (!m_bstaticCoefficient){
        throw std::runtime_error("The parameter staticCoefficient is not set");
    } else if (!m_bwritingFreq) {
        throw std::runtime_error("The parameter writingFreq is not set");
    } else if (!m_bnt) {
        throw std::runtime_error("The parameter nt is not set");
    } else if (!m_breleaseTime) {
        throw std::runtime_error("The parameter releaseTime is not set");
    } else if (!m_bfn) {
        throw std::runtime_error("The parameter fn is not set");
    } else if (!m_bns) {
        throw std::runtime_error("The parameter ns is not set");
    } else if (!m_btau) {
        throw std::runtime_error("The parameter tau is not set");
    } else if (!m_bd) {
        throw std::runtime_error("The parameter d is not set");
    } else if (!m_bE) {
        throw std::runtime_error("The parameter E is not set");
    } else if (!m_bk) {
        throw std::runtime_error("The parameter k is not set");
    } else if (!m_bnu) {
        throw std::runtime_error("The parameter nu is not set");
    } else if (!m_bhZ) {
        throw std::runtime_error("The parameter hZ is not set");
    } else if (!m_bdensity) {
        throw std::runtime_error("The parameter density is not set");
    } else if (!m_bstep) {
        throw std::runtime_error("The parameter step is not set");
    } else if (!m_bgrooveSize) {
        throw std::runtime_error("The parameter grooveSize is not set");
    } else if (!m_bgrooveHeight) {
        throw std::runtime_error("The parameter grooveHeight is not set");
    } else if (!m_bvD) {
        throw std::runtime_error("The parameter vD is not set");
    } else if (!m_bpK) {
        throw std::runtime_error("The parameter pK is not set");
    } else if (!m_bpusherStartHeight) {
        throw std::runtime_error("The parameter pusherStartHeight is not set");
    } else if (!m_bpusherEndHeight) {
        throw std::runtime_error("The parameter pusherEndHeight is not set");
    } else if (!m_bwriteNodePositionInterface) {
        throw std::runtime_error("The parameter writeNodePositionInterface is not set");
    } else if (!m_bwriteNodeVelocityInterface) {
        throw std::runtime_error("The parameter writeNodeVelocityInterface is not set");
    } else if (!m_bwriteNodeSpringsAttachedInterface) {
        throw std::runtime_error("The parameter writeNodeSpringsAttachedInterface is not set");
    } else if (!m_bwriteNodePositionAll) {
        throw std::runtime_error("The parameter writeNodePositionAll is not set");
    } else if (!m_bwriteNodeVelocityAll) {
        throw std::runtime_error("The parameter writeNodeVelocityAll is not set");
    } else if (!m_bwriteTotalEnergyAll) {
        throw std::runtime_error("The parameter writeTotalEnergyAll is not set");
    } else if (!m_bwriteTotalForceAll) {
        throw std::runtime_error("The parameter writeTotalForceAll is not set");
    } else if (!m_bwritePusherForce) {
        throw std::runtime_error("The parameter writePusherForce is not set");
    } else if (!m_bwriteNormalForce) {
        throw std::runtime_error("The parameter writeNormalForce is not set");
    } else if (!m_bwriteShearForce) {
        throw std::runtime_error("The parameter writeShearForce is not set");
    } else if (!m_bwriteXYZ) {
        throw std::runtime_error("The parameter writeXYZ is not set");
    } else if (!m_bwriteBeamTorque) {
        throw std::runtime_error("The parameter writeBeamTorque is not set");
    } else if (!m_bwriteBeamShearForce) {
        throw std::runtime_error("The parameter writeBeamShearForce is not set");
    } else if (!m_bfreqNodePositionInterface) {
        throw std::runtime_error("The parameter freqNodePositionInterface is not set");
    } else if (!m_bfreqNodeVelocityInterface) {
        throw std::runtime_error("The parameter freqNodeVelocityInterface is not set");
    } else if (!m_bfreqNodeSpringsAttachedInterface) {
        throw std::runtime_error("The parameter freqNodeSpringsAttachedInterface is not set");
    } else if (!m_bfreqNodePositionAll) {
        throw std::runtime_error("The parameter freqNodePositionAll is not set");
    } else if (!m_bfreqNodeVelocityAll) {
        throw std::runtime_error("The parameter freqNodeVelocityAll is not set");
    } else if (!m_bfreqTotalEnergyAll) {
        throw std::runtime_error("The parameter freqTotalEnergyAll is not set");
    } else if (!m_bfreqTotalForceAll) {
        throw std::runtime_error("The parameter freqTotalForceAll is not set");
    } else if (!m_bfreqPusherForce) {
        throw std::runtime_error("The parameter freqPusherForce is not set");
    } else if (!m_bfreqNormalForce) {
        throw std::runtime_error("The parameter freqNormalForce is not set");
    } else if (!m_bfreqShearForce) {
        throw std::runtime_error("The parameter freqShearForce is not set");
    } else if (!m_bfreqBeamShearForce) {
        throw std::runtime_error("The parameter freqBeamShearForce is not set");
    } else if (!m_bfreqBeamTorque) {
        throw std::runtime_error("The parameter freqBeamTorque is not set");
    } else if (!m_bfreqXYZ) {
        throw std::runtime_error("The parameter freqXYZ is not set");
    } else if (!m_bbeamMass) {
        throw std::runtime_error("The parameter beamMass is not set");
    } else if (!m_bbeamAngle) {
        throw std::runtime_error("The parameter beamAngle is not set");
    } else if (!m_bbeamRotTime) {
        throw std::runtime_error("The parameter beamRotTime is not set");
    } else if (!m_blatticeFilename) {
        throw std::runtime_error("The parameteter latticefilename is not set");
    }
}

#endif /* PARAMETERS_CPP */
