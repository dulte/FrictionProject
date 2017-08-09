#ifndef DATADUMPER_H
#define DATADUMPER_H

#include <memory>
class FrictionInfo;
class LatticeInfo;
class SidePotentialLoading;

class DataDumper
{
public:
    DataDumper(const std::string& outputPath);
    virtual ~DataDumper();
    void dumpFrictionInfo(const std::shared_ptr<FrictionInfo>);
    void dumpLatticeInfo(const std::shared_ptr<LatticeInfo>);
    void dumpSystem(const SidePotentialLoading*);

    std::string frictionInfoPath;
    std::string latticeInfoPath;
    std::string systemPath;
    std::string outputPath;
    std::string dumpPath;
};


#endif /* DATADUMPER_H */
