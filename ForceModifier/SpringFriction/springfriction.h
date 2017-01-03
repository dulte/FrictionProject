#pragma once

#include <vector>
#include "ForceModifier/forcemodifier.h"
#include "FrictionInfo/frictioninfo.h"
#include "DataOutput/dumpable.h"
#include <random>


class SpringFriction : public ForceModifier, public Dumpable
{
public:
    SpringFriction(std::shared_ptr<FrictionInfo>);

    void initialize();
    vec3 getForceModification();
    //void fileOutputAction(std::shared_ptr<H5::H5File>);

    std::shared_ptr<Node> neighbor;

    std::vector<double> m_x0; // Acual position of node connection point
    std::vector<double> m_xOffset; // Offset from node center position
    std::vector<double> m_tReattach;
    std::vector<bool> m_isConnected;
    std::vector<double> m_k;
    std::vector<double> m_fs;
    std::vector<double> m_fk;
    double m_tau;
    double m_coverage;
    int m_ns;
    double m_fnAvg;
    double m_kNormal;
    bool isLockSprings = false;
    double m_numSpringsAttached = 0;
    double m_normalForce = 0;

    static std::random_device rd;
    static thread_local std::mt19937 gen;

    std::shared_ptr<FrictionInfo> m_frictionInfo;
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
};
