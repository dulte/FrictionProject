#ifndef DUMPABLE_H
#define DUMPABLE_H

#include "datapacket.h"

class Dumpable
{
public:
    Dumpable();
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) = 0;
};

#endif // DUMPABLE_H
