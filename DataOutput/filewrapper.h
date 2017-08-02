#ifndef FILEWRAPPER_H
#define FILEWRAPPER_H

#include <iostream>
#include <fstream>
#include "datapacket.h"

class FileWrapper
{
public:
    FileWrapper();
    virtual ~FileWrapper();
    void open(const std::string &path, std::ios_base::openmode mode);
    bool good() const {return stream.good();}
    void write(const DataPacket&);
    std::ofstream stream;
    unsigned int period = 1;
    std::string fpath;
    std::string name;
    std::ios_base::openmode modes;
    bool is_open = false;
};


#endif /* FILEWRAPPER_H */
