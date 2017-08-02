#ifndef DATAPACKETHANDLER_H
#define DATAPACKETHANDLER_H

#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include "datapacket.h"
#include "InputManagement/Parameters/parameters.h"

class FileWrapper;
class SidePotentialLoading;
class DataPacketHandler
{
public:
  DataPacketHandler(const std::string &outputfolder, std::shared_ptr<Parameters> pParameters);
    ~DataPacketHandler();
    void step(std::vector<DataPacket> packets);
    void dumpXYZ(const std::string& xyzstring);
    void dumpSnapshot(std::vector<DataPacket> packets, const std::string& xyz);
    bool doDumpXYZ(int timestep) const {return doWriteXYZ && timestep%freqXYZ == 0;};

private:
    void addBinary(DataPacket::dataId, const std::string &path);
    std::string outputDirectory;
    std::string snapshotDirectory;
    std::shared_ptr<Parameters> parameters;
    std::map<DataPacket::dataId, std::unique_ptr<FileWrapper>> fileMap;
    bool doWriteXYZ;
    std::ofstream ofXYZ;
    unsigned int freqXYZ;
};


#endif // DATAPACKETHANDLER_H
