#ifndef SIDEPOTENTIALLOADING_H
#define SIDEPOTENTIALLOADING_H

#include <memory.h>
#include <vector>
#include <fstream>
#include <string>
#include "DriverBeam/driverbeam.h"
#include "Lattice/UnstructuredLattice/unstructuredlattice.h"
#include "DataOutput/datapackethandler.h"

class SpringFriction;
class PotentialPusher;
class UnstructuredLattice;
class Parameters;

class SidePotentialLoading : public Dumpable
{
public:
    SidePotentialLoading(std::shared_ptr<Parameters> parameters);
    ~SidePotentialLoading();
    void addPusher(double tInit);
    void startDriving();
    void addDriver();
    void isLockFrictionSprings(bool);
    void step(double step, unsigned int timestep);
    virtual std::vector<DataPacket> getDataPackets(int timestep, double time) override;
    virtual std::string xyzString(double time) const;
    bool doDumpSnapshot(double step, unsigned int timestep);

    std::vector<std::shared_ptr<SpringFriction>> frictionElements;
    std::vector<std::shared_ptr<PotentialPusher>> pusherNodes;
    std::shared_ptr<Lattice> m_lattice;
    std::shared_ptr<Parameters> m_parameters;
    std::shared_ptr<DriverBeam> m_driverBeam;

    double m_loadingForce;
    double m_mass;
    double m_kappa;
    double m_eta;
    double m_alpha;

protected:
    int                                m_pusherStartHeight;
    int                                m_pusherEndHeight;
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
};

#endif // SIDEPOTENTIALLOADING_H
