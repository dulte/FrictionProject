#include "frictioninfo.h"

FrictionInfo::FrictionInfo()
{
    ns = 10;
    nx = 57;
    double Fn = 1920;
    fs = Fn/nx/ns*0.4;
    k = fs/1e-6;

    fk = fs*0.4;

    fnAvg = Fn/ns/nx;
    tau = 0.002;
    kNormal = 4e6/ns;
}
