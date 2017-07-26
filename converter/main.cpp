#include <iostream>
#include <memory>
#include "converter.h"

int main(int argc, char *argv[])
{
    std::string parametersPath = "../input/parameters.txt";
    std::string srcPath = "../output/";
    std::string dstPath = "../output/converted.xyz";
    Converter converter(srcPath, dstPath);
    converter.readParameters(parametersPath);
    try{
        converter.convert({"position", "force"});
    } catch (std::exception &ex) {
        std::cerr << "Error: " <<ex.what() << std::endl;
        return -1;
    }
    return 0;
}
