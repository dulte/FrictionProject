
#include "datapacket.h"

DataPacket::DataPacket(DataPacket::dataId id, int timeStep, double time) :
    m_id(id),
    m_timeStep(timeStep),
    m_time(time)
{

}

void DataPacket::push_back(double number)
{
    m_data.push_back(number);
}
