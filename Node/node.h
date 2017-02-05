#pragma once
#include "Lattice/lattice.h"
#include "NodeInfo/nodeinfo.h"
#include "Vec3/vec3.h"
#include "ForceModifier/forcemodifier.h"

#include <memory>
#include <vector>
#include <memory>

using namespace std;


//class node
//{
//public:
//    node();
//};



class NodeInfo;
class Lattice;
class LatticeInfo;


class Node : public std::enable_shared_from_this<Node>
{
public:
    Node(vec3 r, double mass, double momentOfInertia, std::shared_ptr<LatticeInfo> latticeInfo);

    void updateForcesAndMoments();
    void step(double dt);
    void vvstep1(double dt);
    void vvstep2(double dt);
    bool connectToNode(std::shared_ptr<Node> other);
    double distanceTo(Node & other);
    double distanceTo(std::shared_ptr<Node> other);
    void setPhi(double phi);
    void pertubatePosition(vec3 r);

    vec3 & r()           {return m_r;}
    vec3 v()           {return m_v;}
    vec3 f()           {return m_f;}
    vec3 r_offset()    {return m_r_offset;}
    double t();
    double phi()            {return m_phi;}
    double omega()          {return m_omega;}
    double moment()         {return m_moment;}
    double mass()           {return m_mass;}
    double momentOfInertia(){return m_momentOfInertia;}
    const std::vector<std::unique_ptr<NodeInfo>> & getNeighborInfo() const {return neighborInfo;}
    double numNeighbors()   {return neighborInfo.size();}
    void addModifier(std::shared_ptr<ForceModifier> modifier);
    void clearModifiers();
    void isSetForce(bool);
    void setLattice(std::shared_ptr<Lattice> lattice) {m_lattice = lattice;}

protected:
    vec3 m_r;
    vec3 m_r_offset;
    vec3 m_v;
    vec3 m_f;
    double m_phi;
    double m_omega;
    double m_mass;
    double m_moment;
    double m_momentOfInertia;
    bool m_isSetForce;
    std::shared_ptr<LatticeInfo> m_latticeInfo;
    std::shared_ptr<Lattice> m_lattice;

    std::vector<std::unique_ptr<NodeInfo>> neighborInfo;
    std::vector<std::shared_ptr<ForceModifier>> m_modifiers;
};


