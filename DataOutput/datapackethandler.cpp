#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdexcept>
#include "datapackethandler.h"
#include "filewrapper.h"
#include "FrictionSystem/SidePotentialLoading/sidepotentialloading.h"
#include "mkdir.h"

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

void makeDirectory(const std::string& path){
    int rc = mkpath(path.c_str(), 0755);
    if (rc != 0){
        fprintf(stderr, "Failed to create (%d: %s): %s\n",
                errno, strerror(errno), path.c_str());
        throw std::runtime_error("Failed to make directory");
    }
}

DataPacketHandler::DataPacketHandler(const std::string &outputFolder, std::shared_ptr<Parameters> pParameters)
{
    outputDirectory = outputFolder;
    if (outputDirectory.back() != '/') {
        outputDirectory += '/';
    }
    snapshotDirectory = outputDirectory+"snapshot/";
    // Create the output and snapshot directories
    makeDirectory(outputDirectory);
    makeDirectory(snapshotDirectory);
    parameters = pParameters;
    // Handle binary files
    addBinary(DataPacket::dataId::INTERFACE_POSITION         , "interfacePosition");
    addBinary(DataPacket::dataId::INTERFACE_VELOCITY         , "interfaceVelocity");
    addBinary(DataPacket::dataId::INTERFACE_ATTACHED_SPRINGS , "interfaceAttachedSprings");
    addBinary(DataPacket::dataId::INTERFACE_NORMAL_FORCE     , "interfaceNormalForce");
    addBinary(DataPacket::dataId::INTERFACE_SHEAR_FORCE      , "interfaceShearForce");
    addBinary(DataPacket::dataId::ALL_POSITION               , "allPosition");
    addBinary(DataPacket::dataId::ALL_VELOCITY               , "allVelocity");
    addBinary(DataPacket::dataId::ALL_ENERGY                 , "allEnergy");
    addBinary(DataPacket::dataId::ALL_FORCE                  , "allForce");
    addBinary(DataPacket::dataId::BEAM_SHEAR_FORCE           , "beamShearForce");
    addBinary(DataPacket::dataId::BEAM_TORQUE                , "beamTorque");
    addBinary(DataPacket::dataId::PUSHER_FORCE               , "pusherForce");
    // Handle xyz files
    doWriteXYZ = parameters->get<bool>("writeXYZ");
    if (doWriteXYZ){
        ofXYZ.open(outputDirectory+"model.xyz", std::ofstream::out);
        freqXYZ = parameters->get<int>("freqXYZ");
    }
}

DataPacketHandler::~DataPacketHandler()
{
    for(auto& element: fileMap)
        element.second->stream.close();
}

void DataPacketHandler::addBinary(DataPacket::dataId id, const std::string &name){
    std::string path = outputDirectory + name + ".bin";
    std::string Name = name;
    Name[0] = toupper(name[0]);

    auto file = make_unique<FileWrapper>();
    file->name = name;
    file->period = parameters->get<int>("freq"+Name);
    if (parameters->get<bool>("write"+Name))
        file->open(path, std::ios::out | std::ios::binary);
    fileMap[id] = std::move(file);
}

void DataPacketHandler::step(std::vector<DataPacket> packets)
{
    for (DataPacket & packet: packets)
        fileMap[packet.id()]->write(packet);
}

void DataPacketHandler::dumpXYZ(const std::string &xyzstring){
    ofXYZ << xyzstring;
}

void DataPacketHandler::dumpSnapshot(std::vector<DataPacket> packets,
                                     const std::string& xyz){
    // Create a map from all types of packets to the files in the snapshot directory
    std::map<DataPacket::dataId, std::unique_ptr<FileWrapper>> snapshotFiles;
    for(const auto& element: fileMap){
        snapshotFiles[element.first] = make_unique<FileWrapper>();
        snapshotFiles[element.first]->open(snapshotDirectory+element.second->name+".bin",
                                          element.second->modes);
    }
    std::cout << "Writing packets " << packets.size() << std::endl;
    std::cout << "Writing xyz" << xyz.size() << std::endl;
    for(const auto& packet: packets)
        snapshotFiles[packet.id()]->write(packet);
    for(auto& element: snapshotFiles)
        element.second->stream.close();

    std::ofstream xyzStream;
    xyzStream.open(snapshotDirectory+"model.xyz", std::ofstream::out);
    xyzStream << xyz;
    xyzStream.close();
}
