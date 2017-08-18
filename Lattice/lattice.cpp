#include <omp.h>
#include "lattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "LatticeInfo/latticeinfo.h"
#include "DataOutput/datapacket.h"

#define pi 3.14159265358979323

Lattice::Lattice()
{

}

Lattice::~Lattice()
{
}


void Lattice::step(double dt)
{
    omp_set_num_threads( 4 );
#pragma omp flush(dt)

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep(dt);
    }

    m_t += dt*0.5;

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->updateForcesAndMoments();
    }

#pragma omp parallel for
    for (size_t i = 0; i<nodes.size(); i++)
    {
        nodes[i]->vvstep(dt);
    }
    m_t += dt*0.5;
}

std::shared_ptr<LatticeInfo> Lattice::latticeInfoFromParameters(std::shared_ptr<Parameters> parameters){
    double E  = parameters->get<double>("E");
    double nu = parameters->get<double>("nu");
    double d  = parameters->get<double>("d");
    double hZ = parameters->get<double>("hZ");
    return std::make_shared<LatticeInfo>(E, nu, d, hZ);
}

std::shared_ptr<Node> Lattice::newNode(std::shared_ptr<Parameters>  parameters,
                                       std::shared_ptr<LatticeInfo> latticeInfo,
                                       double x, double y) {
    double d  = parameters->get<double>("d");
    double hZ = parameters->get<double>("hZ");
    double density = parameters->get<double>("density");
    double mass = density * d * d * hZ/ 4* pi;
    double momentOfInertia = d*d / 8;
    vec3 pos(x, y, 0);
    std::shared_ptr<Node> node = std::make_shared<Node>(pos, mass, momentOfInertia, latticeInfo);
    return node;
}

std::vector<DataPacket> Lattice::getDataPackets(int timestep, double time){
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();

    DataPacket position_interface_packet = DataPacket(DataPacket::dataId::INTERFACE_POSITION, timestep, time);
    DataPacket velocity_interface_packet = DataPacket(DataPacket::dataId::INTERFACE_VELOCITY, timestep, time);
    DataPacket position_all = DataPacket(DataPacket::dataId::ALL_POSITION, timestep, time);
    DataPacket velocity_all = DataPacket(DataPacket::dataId::ALL_VELOCITY, timestep, time);
    DataPacket force_all    = DataPacket(DataPacket::dataId::ALL_FORCE, timestep, time);
//    DataPacket energy_all = DataPachet(DataPacket::dataId::NODE_TOTAL_ENERGY_ALL,timestep,time);

    for (std::shared_ptr<Node> node : bottomNodes)
    {
        position_interface_packet.push_back(node->r().x());
        position_interface_packet.push_back(node->r().y());
        velocity_interface_packet.push_back(node->v().x());
        velocity_interface_packet.push_back(node->v().y());
    }

    for (std::shared_ptr<Node> node : nodes) {
        position_all.push_back(node->r().x());
        position_all.push_back(node->r().y());
        velocity_all.push_back(node->v().x());
        velocity_all.push_back(node->v().y());
        force_all.push_back(node->f().x());
        force_all.push_back(node->f().y());
    }
    packetvec.push_back(position_interface_packet);
    packetvec.push_back(velocity_interface_packet);
    packetvec.push_back(position_all);
    packetvec.push_back(velocity_all);
    packetvec.push_back(force_all);
    return packetvec;
}

std::string Lattice::xyzRepresentation(){
    std::stringstream xyz;
    // Header and empty comment
    xyz << nodes.size() << "\n\n";
    for(auto& node: nodes){
        vec3 r = node->r();
        xyz << "N" << " " << r[0] << " " << r[1] << '\n';
    }
    return xyz.str();
}
