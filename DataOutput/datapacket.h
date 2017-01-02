#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <vector>



class DataPacket
{
public:
    enum class dataId {
        NODE_POSITION_INTERFACE,
        NODE_VELOCITY_INTERFACE,
        NODE_POSITION_ALL,
        NODE_VELOCITY_ALL,
        NODE_TOTAL_ENERGY_ALL,
        NODE_TOTAL_FORCE_ALL,
        NODE_SPRINGS_ATTACHED_INTERFACE,
        PUSHER_FORCE,
    };

    DataPacket(DataPacket::dataId id, int timeStep, double time);
    void push_back(double number);

    dataId id() {return m_id;}
    int timestep() {return m_timeStep;}
    double time() {return m_time;}
    std::vector<double> data() {return m_data;}

private:
    dataId m_id;
    int m_timeStep;
    double m_time;

    std::vector<double> m_data;
};

#endif // DATAPACKET_H
