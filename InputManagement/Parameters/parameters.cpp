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
            m_nx = static_cast<int>(atof(tokens[1].c_str()));
            m_bnx = 1;
        } else if (tokens[0] == "ny") {
            m_ny = static_cast<int>(atof(tokens[1].c_str()));
            m_bny = 1;
        } else  if (tokens[0] == "writingFreq") {
            m_writingFreq = static_cast<int>(atof(tokens[1].c_str()));
            m_bwritingFreq = 1;
        } else  if (tokens[0] == "nt") {
          m_nt = static_cast<int>(atof(tokens[1].c_str()));
            m_bnt = 1;
        } else  if (tokens[0] == "releaseTime") {
            m_releaseTime = static_cast<int>(atof(tokens[1].c_str()));
            m_breleaseTime = 1;
        } else  if (tokens[0] == "fn") {
            m_fn = atof(tokens[1].c_str());
            m_bfn = 1;
        } else  if (tokens[0] == "ns") {
            m_ns = atof(tokens[1].c_str());
            m_bns = 1;
        } else  if (tokens[0] == "tau") {
            m_tau = atof(tokens[1].c_str());
            m_btau = 1;
        } else  if (tokens[0] == "d") {
            m_d = atof(tokens[1].c_str());
            m_bd = 1;
        } else  if (tokens[0] == "E") {
            m_E = atof(tokens[1].c_str());
            m_bE = 1;
        } else  if (tokens[0] == "k") {
            m_k = atof(tokens[1].c_str());
            m_bk = 1;
        } else  if (tokens[0] == "nu") {
            m_nu = atof(tokens[1].c_str());
            m_bnu = 1;
        } else  if (tokens[0] == "hZ") {
            m_hZ = atof(tokens[1].c_str());
            m_bhZ = 1;
        } else  if (tokens[0] == "density") {
            m_density = atof(tokens[1].c_str());
            m_bdensity = 1;
        } else  if (tokens[0] == "step") {
            m_step = atof(tokens[1].c_str());
            m_bstep = 1;
        } else  if (tokens[0] == "grooveSize") {
            m_grooveSize = static_cast<int>(atof(tokens[1].c_str()));
            m_bgrooveSize = 1;
        } else  if (tokens[0] == "grooveHeight") {
            m_grooveHeight = static_cast<int>(atof(tokens[1].c_str()));
            m_bgrooveHeight = 1;
        } else  if (tokens[0] == "vD") {
            m_vD = atof(tokens[1].c_str());
            m_bvD = 1;
        } else  if (tokens[0] == "pK") {
            m_pK = atof(tokens[1].c_str());
            m_bpK = 1;
        } else  if (tokens[0] == "pusherStartHeight") {
            m_pusherStartHeight = static_cast<int>(atof(tokens[1].c_str()));
            m_bpusherStartHeight = 1;
        } else  if (tokens[0] == "pusherEndHeight") {
            m_pusherEndHeight = static_cast<int>(atof(tokens[1].c_str()));
            m_bpusherEndHeight = 1;
        } else  if (tokens[0] == "writeNodePositionInterface") {
            m_writeNodePositionInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNodePositionInterface = 1;
        } else  if (tokens[0] == "writeNodeVelocityInterface") {
            m_writeNodeVelocityInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNodeVelocityInterface = 1;
        } else  if (tokens[0] == "writeNodeSpringsAttachedInterface") {
            m_writeNodeSpringsAttachedInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNodeSpringsAttachedInterface = 1;
        } else  if (tokens[0] == "writeNodePositionAll") {
            m_writeNodePositionAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNodePositionAll = 1;
        } else  if (tokens[0] == "writeNodeVelocityAll") {
            m_writeNodeVelocityAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNodeVelocityAll = 1;
        } else  if (tokens[0] == "writeTotalEnergyAll") {
            m_writeTotalEnergyAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteTotalEnergyAll = 1;
        } else  if (tokens[0] == "writeTotalForceAll") {
            m_writeTotalForceAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteTotalForceAll = 1;
        } else  if (tokens[0] == "writePusherForce") {
            m_writePusherForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bwritePusherForce = 1;
        } else  if (tokens[0] == "writeNormalForce") {
            m_writeNormalForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteNormalForce = 1;
        } else  if (tokens[0] == "writeShearForce") {
            m_writeShearForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteShearForce = 1;
        } else  if (tokens[0] == "writeXYZ") {
            m_writeXYZ = static_cast<int>(atof(tokens[1].c_str()));
            m_bwriteXYZ = 1;
        } else  if (tokens[0] == "freqNodePositionInterface") {
            m_freqNodePositionInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNodePositionInterface = 1;
        } else  if (tokens[0] == "freqNodeVelocityInterface") {
            m_freqNodeVelocityInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNodeVelocityInterface = 1;
        } else  if (tokens[0] == "freqNodeSpringsAttachedInterface") {
            m_freqNodeSpringsAttachedInterface = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNodeSpringsAttachedInterface = 1;
        } else  if (tokens[0] == "freqNodePositionAll") {
            m_freqNodePositionAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNodePositionAll = 1;
        } else  if (tokens[0] == "freqNodeVelocityAll") {
            m_freqNodeVelocityAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNodeVelocityAll = 1;
        } else  if (tokens[0] == "freqTotalEnergyAll") {
            m_freqTotalEnergyAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqTotalEnergyAll = 1;
        } else  if (tokens[0] == "freqTotalForceAll") {
            m_freqTotalForceAll = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqTotalForceAll = 1;
        } else  if (tokens[0] == "freqPusherForce") {
            m_freqPusherForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqPusherForce = 1;
        } else  if (tokens[0] == "freqNormalForce") {
            m_freqNormalForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqNormalForce = 1;
        } else  if (tokens[0] == "freqShearForce") {
            m_freqShearForce = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqShearForce = 1;
        } else  if (tokens[0] == "freqXYZ") {
            m_freqXYZ = static_cast<int>(atof(tokens[1].c_str()));
            m_bfreqXYZ = 1;
        } else if (tokens[0] == "driverSprings_k") {
            m_driverSprings_k = atof(tokens[1].c_str());
            m_bdriverSprings_k = 1;
        } else if (tokens[0] == "attachmentSprings_k") {
            m_attachmentSprings_k = atof(tokens[1].c_str());
            m_battachmentSprings_k = 1;
        } else if (tokens[0] == "driverForce") {
            m_driverForce = atof(tokens[1].c_str());
            m_bdriverForce = 1;
        } else if (tokens[0] == "driverVD") {
            m_driverVD = atof(tokens[1].c_str());
            m_bdriverVD = 1;
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
}

#endif /* PARAMETERS_CPP */
