#ifndef DATADUMPER_H
#define DATADUMPER_H

#include <memory>
class FrictionInfo;
class LatticeInfo;
class FrictionSystem;
class Parameters;
class Lattice;

class DataDumper
{
public:
    DataDumper(const std::string& outputPath);
    virtual ~DataDumper();
    void dumpFrictionInfo(const std::shared_ptr<FrictionInfo>);
    void dumpLatticeInfo(const std::shared_ptr<LatticeInfo>);
    void dumpSystem(const FrictionSystem*);
    void dumpParameters(const std::shared_ptr<Parameters>);
    void dumpLatticeStructure(const std::shared_ptr<Lattice>);

    std::string frictionInfoPath;
    std::string latticeInfoPath;
    std::string systemPath;
    std::string parametersPath;
    std::string latticePath;
    std::string outputPath;
    std::string dumpPath;
};


#endif /* DATADUMPER_H */
