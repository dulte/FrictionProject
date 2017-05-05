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
    double m_ns;
    double m_nx;
    double m_tau;
};
