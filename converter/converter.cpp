#include <iostream>
#include <set>
#include <fstream>
#include <vector>
#include <memory>
#include <numeric>
#include <sys/stat.h>
#include <iterator>
#include <sstream>
#include "converter.h"
#include "InputManagement/Parameters/parameters.h"

using Pair  = std::pair<std::string, FileInfo*>;
void splitLineIntoTokens(std::string &s, std::vector<std::string> &tokens);

int gcd(int a, int b)
{
    for (;;)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int lcm(int a, int b)
{
    int temp = gcd(a, b);

    return temp ? (a / temp * b) : 0;
}

void splitLineIntoTokens(std::string &s, std::vector<std::string> &tokens){
    std::istringstream iss(s);
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         back_inserter(tokens));
}

std::vector<std::string> splitpath(const std::string& str, const std::set<char> delimiters)
{
    std::vector<std::string> result;

    char const* pch = str.c_str();
    char const* start = pch;
    for(; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);
                result.push_back(str);
            }
            else
            {
                result.push_back("");
            }
            start = pch + 1;
        }
    }
    result.push_back(start);

    return result;
}

Converter::Converter(const std::string &srcPath,
                     const std::string &dstPath)
    :srcPath(srcPath),
     dstPath(dstPath)
{
}

Converter::~Converter(){
    for(auto &fileinfo: sources){
        fileinfo->stream.close();
        delete[] fileinfo->buffer;
    }
    ofXYZ.close();
}
void Converter::readLattice(const std::string &path){
    std::ifstream latticeFile(path);
    if (!latticeFile) {
        throw std::runtime_error("The lattice file could not be read");
    }

    std::string firstLine;
    std::string comment;
    std::string line;
    getline(latticeFile, firstLine);
    getline(latticeFile, comment);
    // parseComment(comment);
    numTotalNodes = std::stoi(firstLine);

    while (getline(latticeFile, line)) {
        std::vector<std::string> tokens;
        splitLineIntoTokens(line, tokens);

        // Accept differing number of columns
        switch (tokens.size()) {
        case 0:
            continue;
        case 3:
            break;
        default: {
            std::cerr << "Warning: A line in " << path << " contained an unexpected number of tokens."
                      << std::endl;
            break;
        }
        }
        // Use the letters in the first column to determine the node type
        if (tokens[0].find("T") != std::string::npos)
            numTopNodes++;
        if (tokens[0].find("B") != std::string::npos)
            numBottomNodes++;
        if (tokens[0].find("L") != std::string::npos)
            numLeftNodes++;
        if (tokens[0].find("N") != std::string::npos)
            numNormalNodes++;
    }
}

void Converter::readParameters(const std::string &path){
    // For future readers: No, I did not design this to be this ugly. It just kinda happened
    Parameters params(path);
    std::set<char> delims{'/'};
    std::string subpath = "";
    std::vector<std::string> splutpath = splitpath(path, delims);
    for(size_t i = 0; i < splutpath.size()-2; i++)
        subpath += splutpath[i]+'/';
    readLattice(subpath+params.m_latticeFilename);
    nx = params.m_nx;
    ny = params.m_ny;
    nt = params.m_nt;
    rt = params.m_releaseTime;
    nodePositionInterface.name        = "node_position_interface.bin";
    nodeVelocityInterface.name        = "node_velocity_interface.bin";
    nodeSpringsAttachedInterface.name = "node_springs_attached_interface.bin";
    nodePositionAll.name              = "node_position_all.bin";
    nodeVelocityAll.name              = "node_total_velocity.bin";
    totalEnergyAll.name               = "node_total_energy_all.bin";
    totalForceAll.name                = "node_force_all.bin";
    pusherForce.name                  = "pusher_force.bin";
    normalForce.name                  = "normal_force.bin";
    shearForce.name                   = "shear_force.bin";
    beamTorque.name                   = "beam_shear_force.bin";
    beamShearForce.name               = "beam_torque.bin";
    nodePositionInterface.frequency        = params.m_freqNodePositionInterface;
    nodeVelocityInterface.frequency        = params.m_freqNodeVelocityInterface;
    nodeSpringsAttachedInterface.frequency = params.m_freqNodeSpringsAttachedInterface;
    nodePositionAll.frequency              = params.m_freqNodePositionAll;
    nodeVelocityAll.frequency              = params.m_freqNodeVelocityAll;
    totalEnergyAll.frequency               = params.m_freqTotalEnergyAll;
    totalForceAll.frequency                = params.m_freqNodeForceAll;
    pusherForce.frequency                  = params.m_freqPusherForce;
    normalForce.frequency                  = params.m_freqNormalForce;
    shearForce.frequency                   = params.m_freqShearForce;
    beamTorque.frequency                   = params.m_freqBeamTorque;
    beamShearForce.frequency               = params.m_freqBeamShearForce;
    nodePositionInterface.size        = numBottomNodes*2;
    nodeVelocityInterface.size        = numBottomNodes*2;
    nodeSpringsAttachedInterface.size = numBottomNodes*2;
    /* Under the assumption that the driver beam is used, the top nodes
      are replaced by the driver beam itself, counting as one node. In
      total the number of nodes are (total-nx+1). Since both x and y are
      written, the number of bytes is 2*(total-nx+1)
    */
    nodePositionAll.size              = (numTotalNodes-numTopNodes+1)*2;
    nodeVelocityAll.size              = (numTotalNodes-numTopNodes+1)*2;
    totalEnergyAll.size               = numTotalNodes;
    totalForceAll.size                = (numTotalNodes-numTopNodes+1)*2;
    pusherForce.size                  = 1;
    normalForce.size                  = numBottomNodes; // Number of springs??
    shearForce.size                   = numBottomNodes;
    beamTorque.size                   = numTopNodes;
    beamShearForce.size               = numTopNodes;
    constructMap();
}

void Converter::constructMap(){
    fileMap.insert(Pair("position", &nodePositionAll));
    fileMap.insert(Pair("force",    &totalForceAll));
    isMapConstructed = true;
}

void Converter::convert(std::initializer_list<std::string> list){
    if(!isMapConstructed)
        throw "fileMap must be constructed before running convert";

    // Select the files to be used.
    for (auto elem: list)
        sources.push_back(fileMap[elem]);
    // For a meaningfull reading, the sizes must be equal
    const size_t block_size = sources[0]->size;
    for (size_t i = 1; i < sources.size(); i++){
        if (sources[i]->size != block_size)
            throw "All sources must have equal block size";
    }
    std::cout << "Writing " << block_size/2 << " nodes each turn" << std::endl;

    // Find a synchronized frequency at which the outputs sync up
    // The synchronized frequency is the least common denomination of all the frequencies.
    std::vector<int> periods;
    for(auto &fileInfo: sources)
        periods.push_back(fileInfo->frequency);
    int period = std::accumulate(periods.begin(), periods.end(), 1, lcm);
    std::cout << "Reading with frequency " << period << std::endl;

    // Open the files
    for (auto &fileInfo: sources){
        struct stat results;
        std::string path = srcPath+fileInfo->name;
        // Check that the file is not empty
        if (stat(path.c_str(), &results) == 0){
            std::cout << "Size of " << path << " is " << results.st_size << " bytes." << std::endl;
            if (results.st_size == 0){
                std::cerr << path << " is empty" << std::endl;
                throw; // I'm a naughty boy
            }
        } else {
            std::cerr << "Could not find size of " << path << std::endl;
            throw;
        }
        fileInfo->stream.open(path, std::ios::in | std::ios::binary);
        if(!fileInfo->stream.is_open())
            throw std::runtime_error("Could not open source file");
    }
    ofXYZ.open(dstPath);

    /* Only the lcm(...)/freq'th block will be read from each stream, ensuring that the streams
       stay synchronized.
       E.g Stream A has freq 2, stream B freq 3 and C freq 4. Then lcm(2,3,4)=12.
       Each 12/2=6'th block of stream A will be combined with
       every 12/3=4'th block of block B and every 12/4=3'rd block of stream C.
    */
    for(auto & fInfo: sources){
        fInfo->offset = period/fInfo->frequency;
        fInfo->buffer = new double[fInfo->size];
        if (fInfo->buffer == nullptr)
            throw "Could not allocate memory";
    }
    size_t blk_nr = 0;
    bool err_flag = false;
    while (!err_flag){
        // All of the buffers must be filled before writing can commence
        for (auto& fInfo : sources){
            /* One block has size blk_nr*size. Normally this is sufficient,
               but to synchronize the streams only every n'th block must be read,
               corresponding to the offset.
            */
            fInfo->stream.seekg(sizeof(double)*blk_nr*fInfo->size*fInfo->offset, std::ios::beg);
            // The error flag is set if any reading fail
            err_flag |= !fInfo->stream.read((char*)(fInfo->buffer), sizeof(double)*(fInfo->size));
        }
        if(err_flag)
            break;

        ofXYZ << block_size/2 << "\n\n";
        for(size_t i = 0; i < block_size; i += 2){
            ofXYZ << "N";
            for(auto& fInfo: sources){
                ofXYZ << ' ' << fInfo->buffer[i] << ' ' << fInfo->buffer[i+1];
            }
            ofXYZ << '\n';
        }
        blk_nr++;
    }
    std::cout << "Read " << (blk_nr-1)*(block_size)*sizeof(double) << " bytes" << std::endl;
    ofXYZ.close();
}

