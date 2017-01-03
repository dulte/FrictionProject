#pragma once
#include "InputManagement/ConfigReader/configreader.h"

class FrictionInfo
{
public:
    FrictionInfo();

    double k;
    double fs;
    double fk;
    double fnAvg;
    double kNormal;
    double ns;
    double nx;
    double tau;

private:
    ConfigReader *input = new ConfigReader("config.txt");
};
