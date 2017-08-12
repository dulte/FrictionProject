#include "frictioninfo.h"
FrictionInfo::FrictionInfo(std::shared_ptr<Parameters> spParameters)
{
  m_ns                = spParameters->get<int>("ns");
  m_nx                = spParameters->get<int>("nx");
  m_meantime          = spParameters->get<double>("tRmean");
  m_stdtime           = spParameters->get<double>("tRstd");
  m_mus               = spParameters->get<double>("mus");
  double Fn           = spParameters->get<double>("fn");
  double k            = spParameters->get<double>("k");


  // fs blir vel helt feil? m_nx burde være antall bottom nodes
  m_fs      = Fn/m_nx/m_ns*m_mus; // f_thres
  m_k       = m_fs/1e-6;
  m_fk      = m_fs*m_mus;
  m_fnAvg   = Fn/m_ns/m_nx;
  m_kNormal = k/m_ns;
}
