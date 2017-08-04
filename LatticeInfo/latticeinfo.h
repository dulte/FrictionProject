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
    double m_kappa_s =0;   // Shear component of beam coefficient
    double m_kappa_n =0;   // Normal component of beam coefficient
    double m_E       =0;   // Young's modulus
    double m_G       =0;   // Shear modulus
    double m_nu      =0;   // Poisson's ratio
    double m_hZ      =0;   // Depth in the z-direction
    double m_hY      =0;   // Width in the y-direction
    double m_I       =0;   // The geometric part of the moment of inertia
    double m_d       =0;
    double m_Phi     =0;
    double m_A       =0;
};
