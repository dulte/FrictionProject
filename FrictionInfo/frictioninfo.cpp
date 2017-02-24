#include "frictioninfo.h"
FrictionInfo::FrictionInfo()
{


    ns = input->get("ns");
    nx = input->get("nx");
    double Fn = input->get("fn");
    tau = input->get("tau");



    fs = Fn/nx/ns*0.4;
    k = fs/1e-6;

    fk = fs*0.4;

    fnAvg = Fn/ns/nx;
    kNormal = 4e6/ns;
}
