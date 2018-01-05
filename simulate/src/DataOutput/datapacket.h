#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <vector>

class DataPacket
{
public:
    enum class dataId {
        INTERFACE_POSITION,
        INTERFACE_VELOCITY,
        INTERFACE_ATTACHED_SPRINGS,
        INTERFACE_NORMAL_FORCE,
        INTERFACE_SHEAR_FORCE,
        ALL_POSITION,
        ALL_VELOCITY,
        ALL_ENERGY,
        ALL_FORCE,
        PUSHER_FORCE,
        BEAM_TORQUE,
        BEAM_SHEAR_FORCE
    };


    DataPacket(DataPacket::dataId id, int timeStep, double time);
    void push_back(double number);

    dataId id()                const {return m_id;}
    int    timestep()          const {return m_timeStep;}
    double time()              const {return m_time;}
    std::vector<double> data() const {return m_data;}

private:
    dataId m_id;
    int    m_timeStep;
    double m_time;

    std::vector<double> m_data;
};

#endif // DATAPACKET_H
