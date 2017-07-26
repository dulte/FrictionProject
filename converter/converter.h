#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <vector>
#include <initializer_list>


struct FileInfo{
    std::ifstream stream; // Stream from which the bytes are read
    int frequency;        // The period at which the bytes were written from simulation
    std::string name;     // The filename
    size_t size;          // The size in number of doubles of one block of data
    size_t offset;        // The offset in bytes needed for synchronizing with the other streams
    double* buffer;       // Buffer containing the data read from file
};

class Converter
{
public:
    Converter(const std::string &srcPath,
              const std::string &dstPath);
    virtual ~Converter();

    void readLattice(const std::string &path);
    void readParameters(const std::string &path);
    void convert(std::initializer_list<std::string>);
    std::map<std::string, FileInfo*> fileMap;

private:
    void constructMap();
    bool isMapConstructed = false;
    std::vector<FileInfo*> sources;
    unsigned int numTopNodes = 0;
    unsigned int numBottomNodes = 0;
    unsigned int numTotalNodes = 0;
    unsigned int numLeftNodes = 0;
    unsigned int numNormalNodes = 0;
    int nx;
    int ny;
    int nt;
    int rt;
    const std::string srcPath;
    const std::string dstPath;
    FileInfo nodePositionInterface;
    FileInfo nodeVelocityInterface;
    FileInfo nodeSpringsAttachedInterface;
    FileInfo nodePositionAll;
    FileInfo nodeVelocityAll;
    FileInfo totalEnergyAll;
    FileInfo totalForceAll;
    FileInfo pusherForce;
    FileInfo normalForce;
    FileInfo shearForce;
    FileInfo beamTorque;
    FileInfo beamShearForce;
    std::ofstream ofXYZ;
    std::ifstream latticeXYZ;
};
