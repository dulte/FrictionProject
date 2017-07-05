#ifndef PARAMETERS_CPP
#define PARAMETERS_CPP

#include "parameters.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>

Parameters::Parameters(std::string filenameConfig){
    readParameters(filenameConfig);
    checkThatAllParametersAreSet();
    m_infileParameters.close();
}

Parameters::~Parameters(){
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
            m_releaseTime = static_cast<int>(std::stoi(tokens[1]));
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
        } else if (tokens[0] == "driverSprings_k") {
            m_driverSprings_k = std::stod(tokens[1]);
            m_bdriverSprings_k = 1;
        } else if (tokens[0] == "attachmentSprings_k") {
            m_attachmentSprings_k = std::stod(tokens[1]);
            m_battachmentSprings_k = 1;
        } else if (tokens[0] == "driverForce") {
            m_driverForce = std::stod(tokens[1]);
            m_bdriverForce = 1;
        } else if (tokens[0] == "driverVD") {
            m_driverVD = std::stod(tokens[1]);
            m_bdriverVD = 1;
        } else if (tokens[0] == "straightenerK") {
            m_straightener_k = std::stod(tokens[1]);
            m_bstraightener_k = 1;
        } else if (tokens[0] == "latticefilename") {
            m_latticeFilename = tokens[1];
            m_blatticeFilename = 1;
        } else if (tokens[0][0] == '\n' || tokens[0][0] == '#') {
            continue;
        } else {
            std::cerr << "Unknown token in config file: " << tokens[0] << std::endl;
        }
    }
}

void Parameters::checkThatAllParametersAreSet(){
    if (!m_bnx) {
        std::cerr << "The parameter nx is not set" << std::endl;
        throw std::runtime_error("The parameter nx is not set");
    } else if (!m_bny) {
        std::cerr << "The parameter ny is not set" << std::endl;
        throw std::runtime_error("The parameter ny is not set");
    } else if (!m_bwritingFreq) {
        std::cerr << "The parameter writingFreq is not set" << std::endl;
        throw std::runtime_error("The parameter writingFreq is not set");
    } else if (!m_bnt) {
        std::cerr << "The parameter nt is not set" << std::endl;
        throw std::runtime_error("The parameter nt is not set");
    } else if (!m_breleaseTime) {
        std::cerr << "The parameter releaseTime is not set" << std::endl;
        throw std::runtime_error("The parameter releaseTime is not set");
    } else if (!m_bfn) {
        std::cerr << "The parameter fn is not set" << std::endl;
        throw std::runtime_error("The parameter fn is not set");
    } else if (!m_bns) {
        std::cerr << "The parameter ns is not set" << std::endl;
        throw std::runtime_error("The parameter ns is not set");
    } else if (!m_btau) {
        std::cerr << "The parameter tau is not set" << std::endl;
        throw std::runtime_error("The parameter tau is not set");
    } else if (!m_bd) {
        std::cerr << "The parameter d is not set" << std::endl;
        throw std::runtime_error("The parameter d is not set");
    } else if (!m_bE) {
        std::cerr << "The parameter E is not set" << std::endl;
        throw std::runtime_error("The parameter E is not set");
    } else if (!m_bk) {
        std::cerr << "The parameter k is not set" << std::endl;
        throw std::runtime_error("The parameter k is not set");
    } else if (!m_bnu) {
        std::cerr << "The parameter nu is not set" << std::endl;
        throw std::runtime_error("The parameter nu is not set");
    } else if (!m_bhZ) {
        std::cerr << "The parameter hZ is not set" << std::endl;
        throw std::runtime_error("The parameter hZ is not set");
    } else if (!m_bdensity) {
        std::cerr << "The parameter density is not set" << std::endl;
        throw std::runtime_error("The parameter density is not set");
    } else if (!m_bstep) {
        std::cerr << "The parameter step is not set" << std::endl;
        throw std::runtime_error("The parameter step is not set");
    } else if (!m_bgrooveSize) {
        std::cerr << "The parameter grooveSize is not set" << std::endl;
        throw std::runtime_error("The parameter grooveSize is not set");
    } else if (!m_bgrooveHeight) {
        std::cerr << "The parameter grooveHeight is not set" << std::endl;
        throw std::runtime_error("The parameter grooveHeight is not set");
    } else if (!m_bvD) {
        std::cerr << "The parameter vD is not set" << std::endl;
        throw std::runtime_error("The parameter vD is not set");
    } else if (!m_bpK) {
        std::cerr << "The parameter pK is not set" << std::endl;
        throw std::runtime_error("The parameter pK is not set");
    } else if (!m_bpusherStartHeight) {
        std::cerr << "The parameter pusherStartHeight is not set" << std::endl;
        throw std::runtime_error("The parameter pusherStartHeight is not set");
    } else if (!m_bpusherEndHeight) {
        std::cerr << "The parameter pusherEndHeight is not set" << std::endl;
        throw std::runtime_error("The parameter pusherEndHeight is not set");
    } else if (!m_bwriteNodePositionInterface) {
        std::cerr << "The parameter writeNodePositionInterface is not set" << std::endl;
        throw std::runtime_error("The parameter writeNodePositionInterface is not set");
    } else if (!m_bwriteNodeVelocityInterface) {
        std::cerr << "The parameter writeNodeVelocityInterface is not set" << std::endl;
        throw std::runtime_error("The parameter writeNodeVelocityInterface is not set");
    } else if (!m_bwriteNodeSpringsAttachedInterface) {
        std::cerr << "The parameter writeNodeSpringsAttachedInterface is not set" << std::endl;
        throw std::runtime_error("The parameter writeNodeSpringsAttachedInterface is not set");
    } else if (!m_bwriteNodePositionAll) {
        std::cerr << "The parameter writeNodePositionAll is not set" << std::endl;
        throw std::runtime_error("The parameter writeNodePositionAll is not set");
    } else if (!m_bwriteNodeVelocityAll) {
        std::cerr << "The parameter writeNodeVelocityAll is not set" << std::endl;
        throw std::runtime_error("The parameter writeNodeVelocityAll is not set");
    } else if (!m_bwriteTotalEnergyAll) {
        std::cerr << "The parameter writeTotalEnergyAll is not set" << std::endl;
        throw std::runtime_error("The parameter writeTotalEnergyAll is not set");
    } else if (!m_bwriteTotalForceAll) {
        std::cerr << "The parameter writeTotalForceAll is not set" << std::endl;
        throw std::runtime_error("The parameter writeTotalForceAll is not set");
    } else if (!m_bwritePusherForce) {
        std::cerr << "The parameter writePusherForce is not set" << std::endl;
        throw std::runtime_error("The parameter writePusherForce is not set");
    } else if (!m_bwriteNormalForce) {
        std::cerr << "The parameter writeNormalForce is not set" << std::endl;
        throw std::runtime_error("The parameter writeNormalForce is not set");
    } else if (!m_bwriteShearForce) {
        std::cerr << "The parameter writeShearForce is not set" << std::endl;
        throw std::runtime_error("The parameter writeShearForce is not set");
    } else if (!m_bwriteXYZ) {
        std::cerr << "The parameter writeXYZ is not set" << std::endl;
        throw std::runtime_error("The parameter writeXYZ is not set");
    } else if (!m_bfreqNodePositionInterface) {
        std::cerr << "The parameter freqNodePositionInterface is not set" << std::endl;
        throw std::runtime_error("The parameter freqNodePositionInterface is not set");
    } else if (!m_bfreqNodeVelocityInterface) {
        std::cerr << "The parameter freqNodeVelocityInterface is not set" << std::endl;
        throw std::runtime_error("The parameter freqNodeVelocityInterface is not set");
    } else if (!m_bfreqNodeSpringsAttachedInterface) {
        std::cerr << "The parameter freqNodeSpringsAttachedInterface is not set" << std::endl;
        throw std::runtime_error("The parameter freqNodeSpringsAttachedInterface is not set");
    } else if (!m_bfreqNodePositionAll) {
        std::cerr << "The parameter freqNodePositionAll is not set" << std::endl;
        throw std::runtime_error("The parameter freqNodePositionAll is not set");
    } else if (!m_bfreqNodeVelocityAll) {
        std::cerr << "The parameter freqNodeVelocityAll is not set" << std::endl;
        throw std::runtime_error("The parameter freqNodeVelocityAll is not set");
    } else if (!m_bfreqTotalEnergyAll) {
        std::cerr << "The parameter freqTotalEnergyAll is not set" << std::endl;
        throw std::runtime_error("The parameter freqTotalEnergyAll is not set");
    } else if (!m_bfreqTotalForceAll) {
        std::cerr << "The parameter freqTotalForceAll is not set" << std::endl;
        throw std::runtime_error("The parameter freqTotalForceAll is not set");
    } else if (!m_bfreqPusherForce) {
        std::cerr << "The parameter freqPusherForce is not set" << std::endl;
        throw std::runtime_error("The parameter freqPusherForce is not set");
    } else if (!m_bfreqNormalForce) {
        std::cerr << "The parameter freqNormalForce is not set" << std::endl;
        throw std::runtime_error("The parameter freqNormalForce is not set");
    } else if (!m_bfreqShearForce) {
        std::cerr << "The parameter freqShearForce is not set" << std::endl;
        throw std::runtime_error("The parameter freqShearForce is not set");
    } else if (!m_bdriverSprings_k) {
        std::cerr << "The parameter driverSprings_k is not set" << std::endl;
        throw std::runtime_error("The parameter driverSprings_k is not set");
    } else if (!m_battachmentSprings_k) {
        std::cerr << "The parameter attachmentSprings_k is not set" << std::endl;
        throw std::runtime_error("The parameter attachmentSprings_k is not set");
    } else if (!m_bdriverForce) {
        std::cerr << "The parameter driverForce is not set" << std::endl;
        throw std::runtime_error("The parameter driverForce is not set");
    } else if (!m_bdriverVD) {
        std::cerr << "The parameter driverVD is not set" << std::endl;
        throw std::runtime_error("The parameter VD is not set");
    } else if (!m_bfreqXYZ) {
        std::cerr << "The parameter freqXYZ is not set" << std::endl;
        throw std::runtime_error("The parameter freqXYZ is not set");
    }
    else if (!m_bstraightener_k) {
        std::cerr << "The parameter straightenerK is not set" << std::endl;
        throw std::runtime_error("The parameter straightenerK is not set");
    } else if (!m_blatticeFilename) {
        throw std::runtime_error("The parameteter latticefilename is not set");
    }
}

#endif /* PARAMETERS_CPP */
