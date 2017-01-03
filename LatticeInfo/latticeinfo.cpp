
#include "latticeinfo.h"
#include <iostream>

#define pi 3.14159265358979323

LatticeInfo::LatticeInfo(double E, double nu, double d, double hZ)
//    : m_E(E),
//      m_nu(nu),
//      m_hZ(hZ),
//      m_hY(d/2),
//      m_d(d)
{
    m_E = input->get("E");
    m_nu = input->get("nu");
    m_d = input->get("d");
    m_hZ = input->get("hZ");
    m_hY = m_d/2.0;

    m_G = m_E/(2*(1+m_nu));
    m_A = m_hZ*m_hY;
    m_I = m_hZ*m_hY*m_hY*m_hY/12;
    m_Phi = 12*m_E*m_I/(m_G*m_d*m_d*m_A);
    m_kappa_s = 12*m_E*m_I/(m_d*m_d*(1+m_Phi));
    m_kappa_n = m_E*m_A/m_d;


    std::cout << "E " << m_E << std::endl;
    std::cout << "G " << m_G << std::endl;
    std::cout << "hY " << m_hY << std::endl;
    std::cout << "I " << m_I << std::endl;
    std::cout << "Phi " << m_Phi << std::endl;
    std::cout << "kappa_s " << m_kappa_s << std::endl;
    std::cout << "kappa_n " << m_kappa_n << std::endl;
}
