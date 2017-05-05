#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;


class ConfigReader
{
public:
    ConfigReader(string filename);
    ~ConfigReader();

    double get(string variable_name);

private:
    ifstream infile;
    string line;
};

