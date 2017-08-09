#include <memory>
#include <iostream>
#include <fstream>
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "FrictionInfo/frictioninfo.h"
#include "LatticeInfo/latticeinfo.h"
#include "InputManagement/Parameters/parameters.h"
#include "DataOutput/datapackethandler.h"
#include "datadumper.h"
#include "Lattice/lattice.h"


// void MakeDirectory(const std::string& path){
//     int rc = mkpath(path.c_str(), 0755);
//     if (rc != 0){
//         fprintf(stderr, "Failed to create (%d: %s): %s\n",
//                 errno, strerror(errno), path.c_str());
//         throw std::runtime_error("Failed to make directory");
//     }
// }

DataDumper::DataDumper(const std::string& outputPath)
    :frictionInfoPath("frictionInfo"),
     latticeInfoPath("latticeInfo"),
     systemPath("system"),
     parametersPath("parameters"),
     latticePath("lattice.xyz"),
     dumpPath("info/")
{
    this->outputPath = outputPath;
    if (this->outputPath.back() != '/')
        this->outputPath += '/';
    this->outputPath += dumpPath;
    makeDirectory(this->outputPath);
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

void DataDumper::dumpParameters(const std::shared_ptr<Parameters> params){
    std::ofstream file(outputPath+parametersPath);
    file << *params;
    file.close();
}

void DataDumper::dumpLatticeStructure(const std::shared_ptr<Lattice> lattice){
    std::ofstream file(outputPath+latticePath);
    file << lattice->xyzRepresentation();
    file.close();
}
