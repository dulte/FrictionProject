#include "configreader.h"

ConfigReader::ConfigReader(string filename)
{
    infile.open(filename);
}

ConfigReader::~ConfigReader(){
    infile.close();
}


double ConfigReader::get(string variable_name){
    infile.clear();
    infile.seekg(0,ios::beg);
    for (string l; getline(infile,l);)
    {
        stringstream ss(l);

        string name;
        double var1;

        ss >> name >> var1;

        if (name == variable_name){
            return var1;
        }
    }

    cerr << "Could not find a variable by that name: " << variable_name << endl;
    return 0;
}
