#ifndef DATAPACKETHANDLER_H
#define DATAPACKETHANDLER_H

#include <fstream>
#include <memory>
#include "datapacket.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "Lattice/TriangularLattice/triangularlatticewithgrooves.h"
#include "Lattice/SquareLattice/squarelattice.h"
#include <iostream>
#include "InputManagement/Parameters/parameters.h"

class TriangularLatticeWithGrooves;
class DataPacketHandler
{
public:
  DataPacketHandler(std::string outputfolder, std::shared_ptr<Parameters> pParameters);
    ~DataPacketHandler();
    void step(std::vector<DataPacket> packets);
    void dumpXYZ(const std::shared_ptr<TriangularLatticeWithGrooves>& lattice, int timestep);

private:
    std::shared_ptr<Parameters> m_pParameters;

    std::ofstream m_ofNodePositionInterface;
    std::ofstream m_ofNodeVelocityInterface;
    std::ofstream m_ofNodeSpringsAttachedInterface;
    std::ofstream m_ofNodePositionAll;
    std::ofstream m_ofNodeVelocityAll;
    std::ofstream m_ofTotalEnergyAll;
    std::ofstream m_ofTotalForceAll;
    std::ofstream m_ofPusherForce;
    std::ofstream m_ofNormalForce;
    std::ofstream m_ofShearForce;
    std::ofstream m_ofXYZ;

    bool m_writeNodePositionInterface        = true;
    bool m_writeNodeVelocityInterface        = true;
    bool m_writeNodeSpringsAttachedInterface = true;
    bool m_writeNodePositionAll              = true;
    bool m_writeNodeVelocityAll              = true;
    bool m_writeTotalEnergyAll               = true;
    bool m_writeTotalForceAll                = true;
    bool m_writePusherForce                  = true;
    bool m_writeNormalForce                  = true;
    bool m_writeShearForce                   = true;
    bool m_writeXYZ                          = true;

    int m_freqNodePositionInterface          = 1000;
    int m_freqNodeVelocityInterface          = 1000;
    int m_freqNodeSpringsAttachedInterface   = 1000;
    int m_freqNodePositionAll                = 1000;
    int m_freqNodeVelocityAll                = 1000;
    int m_freqTotalEnergyAll                 = 1000;
    int m_freqTotalForceAll                  = 1000;
    int m_freqPusherForce                    = 1000;
    int m_freqNormalForce                    = 1000;
    int m_freqShearForce                     = 1000;
    int m_freqXYZ                            = 1000;
};


#endif // DATAPACKETHANDLER_H
