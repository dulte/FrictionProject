#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <fstream>
#include <iostream>
#include <sstream>

class Parameters{
public:
    explicit Parameters(std::string filenameConfig);
    ~Parameters();
    void readParameters(std::string filenameConfig);
    void checkThatAllParametersAreSet();

    // Simulation Parameters
    int    m_nx;
    int    m_ny;
    int    m_writingFreq;
    int    m_nt;
    int    m_releaseTime;
    double m_fn;
    double m_ns;
    double m_tau;
    double m_d;
    double m_E;
    double m_k;
    double m_nu;
    double m_hZ;
    double m_density;
    double m_step;

    // Filenames
    std::string m_latticeFilename;

    // Grooves
    int    m_grooveSize;
    int    m_grooveHeight;

    // Pusher
    double m_vD;
    double m_pK;
    int    m_pusherStartHeight;
    int    m_pusherEndHeight;

    // Beam
    double m_driverSprings_k;
    double m_attachmentSprings_k;
    double m_driverVD;
    double m_driverForce;
    double m_straightener_k;

    // Writer Frequencies
    int    m_writeNodePositionInterface;
    int    m_writeNodeVelocityInterface;
    int    m_writeNodeSpringsAttachedInterface;
    int    m_writeNodePositionAll;
    int    m_writeNodeVelocityAll;
    int    m_writeTotalEnergyAll;
    int    m_writeTotalForceAll;
    int    m_writePusherForce;
    int    m_writeNormalForce;
    int    m_writeShearForce;
    int    m_writeXYZ;
    int    m_freqNodePositionInterface;
    int    m_freqNodeVelocityInterface;
    int    m_freqNodeSpringsAttachedInterface;
    int    m_freqNodePositionAll;
    int    m_freqNodeVelocityAll;
    int    m_freqTotalEnergyAll;
    int    m_freqTotalForceAll;
    int    m_freqPusherForce;
    int    m_freqNormalForce;
    int    m_freqShearForce;
    int    m_freqXYZ;

private:
    std::ifstream m_infileParameters;

    // Bools to verify that parameters are set
    // Simulation Parameters
    bool m_bnx                                = 0;
    bool m_bny                                = 0;
    bool m_bwritingFreq                       = 0;
    bool m_bnt                                = 0;
    bool m_breleaseTime                       = 0;
    bool m_bfn                                = 0;
    bool m_bns                                = 0;
    bool m_btau                               = 0;
    bool m_bd                                 = 0;
    bool m_bE                                 = 0;
    bool m_bk                                 = 0;
    bool m_bnu                                = 0;
    bool m_bhZ                                = 0;
    bool m_bdensity                           = 0;
    bool m_bstep                              = 0;

    // Filenames
    bool m_blatticeFilename                   = 0;

    // Grooves
    bool m_bgrooveSize                        = 0;
    bool m_bgrooveHeight                      = 0;

    // Pusher
    bool m_bvD                                = 0;
    bool m_bpK                                = 0;
    bool m_bpusherStartHeight                 = 0;
    bool m_bpusherEndHeight                   = 0;

    // Beam
    bool m_bdriverSprings_k                   = 0;
    bool m_battachmentSprings_k               = 0;
    bool m_bdriverVD                          = 0;
    bool m_bdriverForce                       = 0;
    bool m_bstraightener_k                    = 0;

    // Writer Frequencies
    bool m_bwriteNodePositionInterface        = 0;
    bool m_bwriteNodeVelocityInterface        = 0;
    bool m_bwriteNodeSpringsAttachedInterface = 0;
    bool m_bwriteNodePositionAll              = 0;
    bool m_bwriteNodeVelocityAll              = 0;
    bool m_bwriteTotalEnergyAll               = 0;
    bool m_bwriteTotalForceAll                = 0;
    bool m_bwritePusherForce                  = 0;
    bool m_bwriteNormalForce                  = 0;
    bool m_bwriteShearForce                   = 0;
    bool m_bwriteXYZ                          = 0;
    bool m_bfreqNodePositionInterface         = 0;
    bool m_bfreqNodeVelocityInterface         = 0;
    bool m_bfreqNodeSpringsAttachedInterface  = 0;
    bool m_bfreqNodePositionAll               = 0;
    bool m_bfreqNodeVelocityAll               = 0;
    bool m_bfreqTotalEnergyAll                = 0;
    bool m_bfreqTotalForceAll                 = 0;
    bool m_bfreqPusherForce                   = 0;
    bool m_bfreqNormalForce                   = 0;
    bool m_bfreqShearForce                    = 0;
    bool m_bfreqXYZ                           = 0;
};

#endif /* PARAMETERS_H */
