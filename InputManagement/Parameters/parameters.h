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
    void get(std::string name, T& var){
        if (m_parameters.find(name) == m_parameters.end()){
            std::cerr << name << " is not in parameters" << std::endl;
            throw std::exception();
        }
        m_parameters[name]->copyTo(var);
    }


    std::string m_dumpFilename = "paramdump.txt";


private:
    void constructMap();
    std::ifstream m_infileParameters;
    std::map<std::string, std::unique_ptr<ParameterInterface>> m_parameters;
};

#endif /* PARAMETERS_H */
