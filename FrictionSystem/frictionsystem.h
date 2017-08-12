#ifndef FRICTIONSYSTEM_H
#define FRICTIONSYSTEM_H


#include <memory.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "DataOutput/datapackethandler.h"
#include "DataOutput/dumpable.h"
#include "Lattice/lattice.h"

class SpringFriction;
class PotentialPusher;
class Parameters;
class Node;


class FrictionSystem : public Dumpable
{
public:
    FrictionSystem(std::shared_ptr<Parameters> parameters);
    virtual ~FrictionSystem();
    virtual void        startDriving(double tInit) = 0;
    virtual void        isLockFrictionSprings(bool);
    virtual void        step(double step, unsigned int timestep);
    virtual size_t      numberOfNodes() const;
    virtual double      totalDriverForce() const;
    virtual void        postProcessing(){};
            bool        doDumpSnapshot(double step, unsigned int timestep);
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    virtual std::string xyzString(double time) const;
    virtual std::vector<DataPacket> getDriverPackets(int timestep, double time) const;

    std::vector<std::shared_ptr<SpringFriction>>  frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    std::vector<std::shared_ptr<Node>>            m_driverNodes;
    std::shared_ptr<Lattice>                      m_lattice;
    std::shared_ptr<Parameters>                   m_parameters;

    double m_loadingForce;
    double m_mass;
    double m_kappa;
    double m_eta;
    double m_alpha;

protected:
    double                             m_vD;
    double                             m_k;
    double                             m_maxRecordedDriveForce = 0;
    std::vector<DataPacket>            m_currentPackets;
    std::vector<DataPacket>            m_snapshotPackets;
    std::string                        m_snapshotxyz;
    unsigned int                       m_snapshotBufferTime = 1;
    unsigned int                       m_snapshotBeginTime = 0;
    std::unique_ptr<DataPacketHandler> m_dataHandler;
    bool                               m_newMaximum = false;
    bool                               m_isDriving = false;
};


#endif /* FRICTIONSYSTEM_H */
