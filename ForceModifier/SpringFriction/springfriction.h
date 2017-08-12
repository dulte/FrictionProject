#pragma once

#include <vector>
#include "ForceModifier/forcemodifier.h"
#include "FrictionInfo/frictioninfo.h"
#include "DataOutput/dumpable.h"
#include <random>


class SpringFriction : public ForceModifier
{
public:
    SpringFriction(std::shared_ptr<FrictionInfo>);

    void initialize();
    vec3 getForceModification();


    std::vector<double> m_x0; // Acual position of node connection point
    std::vector<double> m_xOffset; // Offset from node center position
    std::vector<double> m_tReattach;
    std::vector<bool>   m_isConnected;
    std::vector<double> m_k;
    std::vector<double> m_fs;
    std::vector<double> m_fk;
    double              m_meantime;
    double              m_stdtime;
    int                 m_ns;
    double              m_fnAvg;
    double              m_kNormal;
    bool                isLockSprings = false;
    double              m_numSpringsAttached = 0;
    double              m_normalForce = 0;
    double              m_shearForce = 0;

    static std::random_device rd;
    static thread_local std::mt19937 gen;

    std::shared_ptr<FrictionInfo> m_frictionInfo;
};
