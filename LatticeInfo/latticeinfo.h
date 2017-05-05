#pragma once

class LatticeInfo
{
public:
    LatticeInfo(double E, double nu, double d, double hZ);
    double kappa_s() {return m_kappa_s;}
    double kappa_n() {return m_kappa_n;}
    double Phi()     {return m_Phi;}
    double I()       {return m_I;}
    double A, B, C;
private:
    double m_kappa_s =0;
    double m_kappa_n =0;
    double m_E       =0;
    double m_G       =0;
    double m_nu      =0;
    double m_hZ      =0;
    double m_hY      =0;
    double m_I       =0;
    double m_d       =0;
    double m_Phi     =0;
    double m_A       = 0;
};
