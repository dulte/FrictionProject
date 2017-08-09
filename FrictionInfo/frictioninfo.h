#pragma once
#include <memory>
#include "InputManagement/Parameters/parameters.h"

class FrictionInfo
{
public:
    FrictionInfo(std::shared_ptr<Parameters> spParameters);

    double m_k;
    double m_fs;
    double m_fk;
    double m_fnAvg;
    double m_kNormal;
    int m_ns;
    int m_nx;
    double m_meantime;
    double m_stdtime;
    double m_mus;
};
