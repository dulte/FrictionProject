#include <memory>
#include <iostream>
#include <fstream>
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "FrictionInfo/frictioninfo.h"
#include "LatticeInfo/latticeinfo.h"
#include "datadumper.h"

DataDumper::DataDumper(const std::string& outputPath)
    :frictionInfoPath("frictionInfo"),
     latticeInfoPath("latticeInfo"),
     systemPath("system"),
     dumpPath("dump")
{
    this->outputPath = outputPath;
    if (this->outputPath.back() != '/')
        this->outputPath += '/';
    // this->outputPath += dumpPath;
}

DataDumper::~DataDumper(){}


void DataDumper::dumpFrictionInfo(const std::shared_ptr<FrictionInfo> info) {
    std::stringstream ss;
    ss << "k" << '\t' << info->m_k << '\n'
       << "fs" << '\t' << info->m_fs << '\n'
       << "fk" << '\t' << info->m_fk << '\n'
       << "fnAvg" << '\t' << info->m_fnAvg << '\n'
       << "kNormal" << '\t' << info->m_kNormal << '\n'
       << "ns" << '\t' << info->m_ns << '\n'
       << "nx" << '\t' << info->m_nx << '\n'
       << "stdtime" << '\t' << info->m_stdtime  << '\n'
       << "meanitme" << '\t' << info->m_meantime << '\n'
       << "mu_s" << '\t' << info->m_mus << std::endl;

    std::ofstream file(outputPath+frictionInfoPath);
    file << ss.str();
    file.close();
}

void DataDumper::dumpLatticeInfo(const std::shared_ptr<LatticeInfo> info) {
    std::stringstream ss;
    ss << "kappa_s" << '\t' << info->m_kappa_s << '\n'
       << "kappa_n" << '\t' << info->m_kappa_n << '\n'
       << "E" << '\t' << info->m_E << '\n'
       << "G" << '\t' << info->m_G << '\n'
       << "nu" << '\t' << info->m_nu << '\n'
       << "hZ" << '\t' << info->m_hZ << '\n'
       << "hY" << '\t' << info->m_hY << '\n'
       << "I" << '\t' << info->m_I << '\n'
       << "d" << '\t' << info->m_d << '\n'
       << "Phi" << '\t' << info->m_Phi << '\n'
       << "A" << '\t' << info->m_A << std::endl;

    std::ofstream file(outputPath+latticeInfoPath);
    file << ss.str();
    file.close();
}

void DataDumper::dumpSystem(const SidePotentialLoading* system) {
    std::stringstream ss;
    ss << "kappa\t" << system->m_kappa << '\n'
       << "loadingForcePerTopNode\t" << system->m_loadingForce << '\n'
       << "mass\t" << system->m_mass << '\n'
       << "eta\t" << system->m_eta << '\n'
       << "alpha\t" << system->m_alpha << std::endl;

   std::ofstream file(outputPath+systemPath);
   file << ss.str();
   file.close();
}
