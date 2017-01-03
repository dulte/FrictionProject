#include "springfriction.h"
#include "Node/node.h"
#include <math.h>

std::random_device SpringFriction::rd;
thread_local std::mt19937 SpringFriction::gen(SpringFriction::rd());

SpringFriction::SpringFriction(std::shared_ptr<FrictionInfo> frictionInfo)
    : m_frictionInfo(frictionInfo)
{
}

void SpringFriction::initialize()
{
    m_tau = m_frictionInfo->tau; m_ns = m_frictionInfo->ns; m_coverage = 0.5; m_fnAvg = m_frictionInfo->fnAvg; m_kNormal = m_frictionInfo->kNormal;
    vec3 otherR = neighbor->r();

    for (int i = 0; i<m_ns; i++)
    {
        m_x0.push_back(m_node->r().x()+(otherR.x()-m_node->r().x())*static_cast<double>(i+0.5)/m_ns*m_coverage);
        m_xOffset.push_back(0);
        m_tReattach.push_back(0);
        m_isConnected.push_back(true);
        m_k.push_back(m_frictionInfo->k);
        m_fs.push_back(m_frictionInfo->fs);
        m_fk.push_back(m_frictionInfo->fk);
        m_numSpringsAttached ++;
    }
}

vec3 SpringFriction::getForceModification()
{
    vec3 offsetVector = neighbor->r()-m_node->r();
    vec3 resultantForce(0, 0, 0);
    m_normalForce = 0;
    for (int i = 0; i<m_ns; i++)
    {
        vec3 springPositionOnNode = m_node->r()+offsetVector*(i+0.5)/m_ns*m_coverage;
        double fn = 0;
        double ft = 0;
        if (springPositionOnNode.y() < 0)
        {
            fn = -springPositionOnNode.y()*m_kNormal;
            m_normalForce += fn;
            if (m_isConnected[i])
            {
                ft = -(springPositionOnNode.x()-m_x0[i])*m_k[i]*sqrt(fn/m_fnAvg);
                if (fabs(ft) > m_fs[i]*fn/m_fnAvg)
                {
                    if (!isLockSprings)
                    {
                        m_isConnected[i] = false;
                        ft = ft/fabs(ft)*m_fk[i]*fn/m_fnAvg;
                        m_x0[i] = springPositionOnNode.x()+ft/m_k[i]*sqrt(m_fnAvg/fn);
                        std::normal_distribution<> nd(m_frictionInfo->tau, 0.3*m_frictionInfo->tau);
                        m_tReattach[i] = m_node->t() + nd(gen);
                        //std::cout << "Detached spring, " << m_tReattach[i]-m_node->t() << std::endl;
                        m_numSpringsAttached --;
                    }
                }
            }
            else if (!m_isConnected[i])
            {
                ft = -(springPositionOnNode.x()-m_x0[i])*m_k[i]*sqrt(fn/m_fnAvg);
                if (fabs(ft) > m_fk[i]*fn/m_fnAvg)
                {
                    ft = ft/fabs(ft)*m_fk[i]*fn/m_fnAvg;
                    m_x0[i] = springPositionOnNode.x()+ft/m_k[i]*sqrt(m_fnAvg/fn);
                }
                if (m_tReattach[i] < m_node->t())
                {
                    m_isConnected[i] = true;
                    m_x0[i] = m_node->r().x()+(neighbor->r().x()-m_node->r().x())*static_cast<double>(i+0.5)/m_ns*m_coverage;
                    m_numSpringsAttached ++;
                }
            }
            resultantForce += vec3(ft, fn, 0);
        }
        else
        {
            if (!isLockSprings && m_isConnected[i])
            {
                m_isConnected[i] = false;
                m_numSpringsAttached --;
            }
        }
    }
    return resultantForce;
}

std::vector<DataPacket> SpringFriction::getDataPackets(int timestep, double time)
{
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();
    packetvec.push_back(m_numSpringsAttached)
    return packetvec;
}
