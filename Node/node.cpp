#include "node.h"
#include "Lattice/simplelattice.h"
#include "NodeInfo/nodeinfo.h"
#include "LatticeInfo/latticeinfo.h"
#include "Vec3/vec3.h"

#include <memory>
#include <vector>
#include <memory>
#include <cmath>

#define pi 3.14159265358979323

Node::Node(vec3 r, double mass, double momentOfInertia, shared_ptr<LatticeInfo> latticeInfo):
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
    m_f = 0;
    m_moment = 0;

    if (!m_isSetForce){

        for (auto & neighbor : neighborInfo){
            vec3 rDiff = neighbor->node()->r() - r();
            double d0 = neighbor->d0();

            double dij = rDiff.length();
            double neighborPhi = neighbor->node()->phi();
            double phiCorrection = 0;//neighbor->phiOffset();


            double phi_ij           = phi()+phiCorrection;
            double phi_ji           = neighborPhi+phiCorrection;

            // TODO: Remove legacy code?
            // if (phi_ij + phi_ji > pi)
            // {
//                cout << "Something went wrong, and block i deformed" << endl;
//                cout << phi_ij+phi_ji << endl;
//                exit(1);
// //                m_r.print();
//             }
            double fn               = m_latticeInfo->kappa_n()*(dij-d0);
            double fs               = -m_latticeInfo->kappa_s()*0.5*(phi_ij + phi_ji);
            double m                = -m_latticeInfo->kappa_s()*dij*(m_latticeInfo->Phi()/12.0*(phi_ij-phi_ji)+0.5*(2.0/3.0*phi_ij+1.0/3.0*phi_ji));

            m_moment += m;
            m_f += rDiff/dij*fn +vec3(-rDiff[0], rDiff[1],0)*fs/dij;
        }
    }

    for (auto & modifier : m_modifiers)
    {
        m_f += modifier->getForceModification();
        m_moment += modifier->getMomentModification();
    }
}


void Node::step(double dt)
{
    m_omega = m_omega + (m_moment/m_momentOfInertia)*dt;
    m_phi = m_phi + m_omega*dt;
    m_v = m_v + (m_f/m_mass)*dt;
    m_r = m_r + m_v*dt;
}

void Node::vvstep1(double dt)
{
    m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    m_phi += m_omega*dt;
    m_v += (m_f/m_mass)*0.5*dt;
    m_r += m_v*dt;
}

void Node::vvstep2(double dt)
{
    m_omega += (m_moment/m_momentOfInertia)*0.5*dt;
    m_phi += m_omega*dt;
    m_v += (m_f/m_mass)*0.5*dt;
    m_r += m_v*dt;
}

bool Node::connectToNode(std::shared_ptr<Node> other)
{
    vec3 rDiff = other->r()-r();
    double phiDiff = atan2(rDiff[1], rDiff[0]);
    double d0 = rDiff.length();
    neighborInfo.push_back(std::make_unique<NodeInfo>(other, d0, phiDiff));
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

double Node::t()
{
    return m_lattice->t();
}

void Node::addModifier(std::shared_ptr<ForceModifier> modifier)
{
    modifier->setNode(shared_from_this());
    modifier->initialize();
    m_modifiers.push_back(move(modifier));
}

void Node::clearModifiers()
{
    m_modifiers.clear();
}

void Node::isSetForce(bool isSetForce)
{
    m_isSetForce = isSetForce;
}
