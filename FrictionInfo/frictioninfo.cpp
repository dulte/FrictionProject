#include "frictioninfo.h"

FrictionInfo::FrictionInfo()
{
//    ns = 10;
//    nx = 57;
//    double Fn = 1920;

    ns = input->get("ns");
    nx = input->get("nx");
    double Fn = input->get("fn");
    tau = input->get("tau");



    fs = Fn/nx/ns*0.4;
    k = fs/1e-6;

    fk = fs*0.4;

    fnAvg = Fn/ns/nx;
    //tau = 0.002;
    kNormal = 4e6/ns;
}
