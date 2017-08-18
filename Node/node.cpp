#include "node.h"
#include "NodeInfo/nodeinfo.h"
#include "LatticeInfo/latticeinfo.h"
#include "Vec3/vec3.h"
#include "Lattice/lattice.h"

#include <iostream>
#include <memory>
#include <vector>
#include <memory>
#include <cmath>

#define pi 3.14159265358979323

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
Node::Node(vec3 r, double mass, double momentOfInertia, std::shared_ptr<LatticeInfo> latticeInfo):
    m_r(r),
    m_v(vec3()),
    m_f(vec3()),
    m_phi(0),
    m_omega(0),
    m_mass(mass),
    m_moment(0),
    m_momentOfInertia(momentOfInertia),
    m_isSetForce(false)
{
    m_latticeInfo = latticeInfo;
    m_momentOfInertia *=m_mass;
}

void Node::updateForcesAndMoments(){
    // Reset all forces
    m_f = 0;
    m_moment = 0;

    if (!m_isSetForce){
        for (auto & neighbor : neighborInfo){
            vec3 rDiff = neighbor->node()->r() - r();
            double d0 = neighbor->d0();
            double phiOffset = neighbor->phiOffset();
            double angleR = atan2(rDiff[1], rDiff[0]);

            double dij = rDiff.length();
            double neighborPhi = neighbor->node()->phi();

            double phi_ij           = angleR - phi() - phiOffset;
            double phi_ji           = angleR - neighborPhi - phiOffset;

            double fn               = m_latticeInfo->kappa_n()*(dij-d0);
            double fs               = -m_latticeInfo->kappa_s()*0.5*(phi_ij + phi_ji);
            double m                = -m_latticeInfo->kappa_s()*dij*(m_latticeInfo->Phi()/12.0*(phi_ij-phi_ji)+0.5*(2.0/3.0*phi_ij+1.0/3.0*phi_ji));

            m_moment += m;
            m_f += rDiff/dij*fn +vec3(-rDiff[0], rDiff[1],0)*fs/dij;
        }
    }
    for (auto & modifier : m_modifiers)
    {
        m_f      += modifier->getForceModification();
        m_moment += modifier->getMomentModification();
    }
}


void Node::step(double dt)
{
    m_omega = m_omega + (m_moment/m_momentOfInertia)*dt;
    m_phi   = m_phi + m_omega*dt;
    m_v     = m_v + (m_f/m_mass)*dt;
    m_r     = m_r + m_v*dt;
}

void Node::vvstep(double dt)
{
    m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    m_phi   += m_omega*dt;
    m_v     += (m_f/m_mass)*0.5*dt;
    m_r     += m_v*dt;
}

void Node::vvstep1(double dt)
{
    m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    m_phi   += m_omega*dt;
    m_v     += (m_f/m_mass)*0.5*dt;
    m_r     += m_v*dt;
}

void Node::vvstep2(double dt)
{
    m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    m_phi   += m_omega*dt;
    m_v     += (m_f/m_mass)*0.5*dt;
    m_r     += m_v*dt;
}

bool Node::connectToNode(std::shared_ptr<Node> other)
{
    vec3 rDiff = other->r()-r();
    double phiDiff = atan2(rDiff[1], rDiff[0]);
    double d0 = rDiff.length();
    neighborInfo.push_back(make_unique<NodeInfo>(other, d0, phiDiff));
    return true;
}

bool Node::connectToNode(std::shared_ptr<Node> other, double distance)
{
    vec3 rDiff = other->r()-r();
    double phiDiff = atan2(rDiff[1], rDiff[0]);
    neighborInfo.push_back(make_unique<NodeInfo>(other, distance, phiDiff));
    return true;
}

double Node::distanceTo(Node & other)
{
    return (other.r()-r()).length();
}

double Node::distanceTo(std::shared_ptr<Node> other)
{
    return (other->r()-r()).length();
}

void Node::setPhi(double phi)
{
    m_phi = phi;
}

void Node::pertubatePosition(vec3 r)
{
    m_r += r;
}

void Node::pertubateRotation(double phi){
    m_phi += phi;
}

double Node::t()
{
    return m_lattice->t();
}

void Node::addModifier(std::shared_ptr<ForceModifier> modifier)
{
    modifier->setNode(shared_from_this());
    modifier->initialize();
    m_modifiers.push_back(std::move(modifier));
}

void Node::clearModifiers()
{
    m_modifiers.clear();
}

void Node::isSetForce(bool isSetForce)
{
    m_isSetForce = isSetForce;
}

void Node::forcePosition(const vec3 &r){
    m_r = r;
}

void Node::forceVelocity(const vec3& v) {
    m_v = v;
}
