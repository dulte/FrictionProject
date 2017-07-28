#include "frictioninfo.h"
FrictionInfo::FrictionInfo(std::shared_ptr<Parameters> spParameters)
{
  m_k                 = spParameters->get<double>("k");
  m_ns                = spParameters->get<int>("ns");
  m_nx                = spParameters->get<int>("nx");
  m_tau               = spParameters->get<double>("tau");
  m_staticCoefficient = spParameters->get<double>("staticCoefficient");
  double Fn           = spParameters->get<double>("fn");

  //TODO: Loads of magic numbers
  m_fs      = Fn/m_nx/m_ns*m_staticCoefficient; //0.4;
  m_k       = m_fs/1e-6; // Hvorfor settes k på nytt her?
  m_fk      = m_fs*m_staticCoefficient;//0.4;
  m_fnAvg   = Fn/m_ns/m_nx;
  m_kNormal = 4e6/m_ns;
}
