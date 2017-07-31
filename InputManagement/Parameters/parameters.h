#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <memory>
#include "parameter.h"
#include <vector>

class Parameters{
public:
    explicit Parameters(const std::string &filenameConfig);
    virtual ~Parameters();
    void readParameters(std::string filenameConfig);
    void checkThatAllParametersAreSet();
    int dumpParameters();
    template <typename T>
    T get(std::string);
    template <typename T>
    void addParameter(std::string name);


private:
    void constructMap();
    std::ifstream m_infileParameters;
    std::map<std::string, std::unique_ptr<Parameter<int>>> m_intparams;
    std::map<std::string, std::unique_ptr<Parameter<std::string>>> m_stringparams;
    std::map<std::string, std::unique_ptr<Parameter<bool>>> m_boolparams;
    std::map<std::string, std::unique_ptr<Parameter<double>>> m_doubleparams;
};

#endif /* PARAMETERS_H */
