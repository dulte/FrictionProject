#include <iostream>
#include <stdexcept>
#include "filewrapper.h"

FileWrapper::FileWrapper(){}
FileWrapper::~FileWrapper(){}

void FileWrapper::open(const std::string &path, std::ios_base::openmode mode){
    fpath = path;
    modes = mode;
    stream.open(fpath, mode);
    if (!(stream.is_open() && good())){
        std::cerr << "Outputfile " << path << " could not be opened" << std::endl;
        throw std::runtime_error("Could not open outputfile.");
    }
    is_open = true;
}

void FileWrapper::write(const DataPacket& packet){
    if (is_open && packet.timestep() % period == 0 && good())
        stream.write((char*)&packet.data()[0], packet.data().size()*sizeof(double));
}
