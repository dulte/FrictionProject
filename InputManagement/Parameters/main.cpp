#include "parameters.h"
#include "parameter.h"
#include <vector>
#include <iostream>
#include <memory>
#include <map>

using Pair = std::pair<std::string, std::unique_ptr<ParameterInterface>>;

template <typename T>
std::unique_ptr<ParameterInterface> newParam(){
    return std::unique_ptr<ParameterInterface>(new Parameter<T>());
}

int main(int argc, char *argv[])
{
    try{
    Parameters parameters("../../input/parameters.txt");
    std::string name;
    int nx;
    double hz;
    parameters.get("hZ", hz);
    parameters.get("nx", nx);
    parameters.get("latticefilename", name);
    std::cout << nx << std::endl;
    std::cout << nx << '\t' << hz << '\t' << name << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
