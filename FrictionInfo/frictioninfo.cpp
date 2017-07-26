#include "frictioninfo.h"
FrictionInfo::FrictionInfo(std::shared_ptr<Parameters> spParameters)
{
  m_k                 = spParameters->m_k;
  m_ns                = spParameters->m_ns;
  m_nx                = spParameters->m_nx;
  m_tau               = spParameters->m_tau;
  m_staticCoefficient = spParameters->m_staticCoefficient;
  double Fn           = spParameters->m_fn;

  //TODO: Loads of magic numbers
  m_fs      = Fn/m_nx/m_ns*m_staticCoefficient; //0.4;
  m_k       = m_fs/1e-6; // Hvorfor settes k på nytt her?
  m_fk      = m_fs*m_staticCoefficient;//0.4;
  m_fnAvg   = Fn/m_ns/m_nx;
  m_kNormal = 4e6/m_ns;
}
