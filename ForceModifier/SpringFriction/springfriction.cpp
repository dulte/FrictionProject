#include "springfriction.h"
#include "Node/node.h"
#include <math.h>
#include <omp.h>

std::random_device SpringFriction::rd;
thread_local std::mt19937 SpringFriction::gen(SpringFriction::rd());

SpringFriction::SpringFriction(std::shared_ptr<FrictionInfo> frictionInfo)
    : m_frictionInfo(frictionInfo)
{
}

void SpringFriction::initialize()
{
    m_tau = m_frictionInfo->tau; m_ns = m_frictionInfo->ns; m_coverage = 0.5; m_fnAvg = m_frictionInfo->fnAvg; m_kNormal = m_frictionInfo->kNormal;

    for (int i = 0; i<m_ns; i++)
    {
        m_x0.push_back(m_node->r().x());
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
    vec3 resultantForce(0, 0, 0);
    m_normalForce = 0;

#pragma omp parallel for
    for (int i = 0; i<m_ns; i++)
    {
        double x_node = m_node->r().x();//+offsetVector*(i+0.5)/m_ns*m_coverage;
        double y_node = m_node->r().y();
        double fn = 0;
        double ft = 0;
        if (y_node < 0)
        {

            fn = -y_node*m_kNormal;

            if (m_isConnected[i])
            {
                ft = -(x_node-m_x0[i])*m_k[i]*sqrt(fn/m_fnAvg);
                if (fabs(ft) > m_fs[i]*fn/m_fnAvg)
                {

                    if (!isLockSprings)
                    {


                        m_isConnected[i] = false;
                        ft = ft/fabs(ft)*m_fk[i]*fn/m_fnAvg;
                        m_x0[i] = x_node+ft/m_k[i]*sqrt(m_fnAvg/fn);
                        std::normal_distribution<> nd(m_frictionInfo->tau, 0.3*m_frictionInfo->tau);
                        m_tReattach[i] = m_node->t() + nd(gen);
                        //std::cout << "Detached spring, " << m_tReattach[i]-m_node->t() << std::endl;
                        m_numSpringsAttached --;
                    }
                }
            }
            else if (!m_isConnected[i])
            {
                ft = -( x_node-m_x0[i])*m_k[i]*sqrt(fn/m_fnAvg);
                if (fabs(ft) > m_fk[i]*fn/m_fnAvg)
                {
                    ft = ft/fabs(ft)*m_fk[i]*fn/m_fnAvg;
                    m_x0[i] =  x_node+ft/m_k[i]*sqrt(m_fnAvg/fn);
                }
                if (m_tReattach[i] < m_node->t())
                {
                    m_isConnected[i] = true;
                    m_x0[i] =  x_node;
                    m_numSpringsAttached ++;
                }
            }
            resultantForce += vec3(ft, fn, 0);
            m_normalForce += fn;
            m_shearForce += ft;
        }
        else
        {
            if (!isLockSprings && m_isConnected[i])
            {
                m_isConnected[i] = false;
                m_numSpringsAttached --;
            }
            if (isLockSprings)
            {
                m_x0[i] = x_node;
            }
        }
    }

    return resultantForce;
}

std::vector<DataPacket> SpringFriction::getDataPackets(int timestep, double time)
{
    std::vector<DataPacket> packetvec = std::vector<DataPacket>();
    DataPacket numberOfSprings = DataPacket(DataPacket::dataId::NODE_SPRINGS_ATTACHED_INTERFACE, timestep, time);
    DataPacket normalForce = DataPacket(DataPacket::dataId::NORMAL_FORCE, timestep, time);
    DataPacket shearForce = DataPacket(DataPacket::dataId::SHEAR_FORCE, timestep, time);

    numberOfSprings.push_back(m_numSpringsAttached);
    normalForce.push_back(m_normalForce);
    shearForce.push_back(m_shearForce);

    packetvec.push_back(numberOfSprings);
    packetvec.push_back(normalForce);
    packetvec.push_back(shearForce);
    return packetvec;
}
