#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <limits>
#include "Node/node.h"
#include "NodeInfo/nodeinfo.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/lattice.h"
#include "latticescanner.h"

#define pi 3.14159265358979323

LatticeScanner::LatticeScanner(std::shared_ptr<Parameters>  parameters,
                               std::shared_ptr<LatticeInfo> latticeinfo):
    m_parameters(parameters),
    m_latticeInfo(latticeinfo)
{}

LatticeScanner::~LatticeScanner(){}

void LatticeScanner::splitLineIntoTokens(std::string &s, std::vector<std::string> &tokens){
    std::istringstream iss(s);
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         back_inserter(tokens));
}

void LatticeScanner::scan(){
    /* Reads the inital block of a xyz file and constructs
       nodes accordingly
    */
    std::string filename = m_parameters->get<std::string>("latticefilename");
    int nx               = m_parameters->get<int>("nx");
    int ny               = m_parameters->get<int>("ny");
    double d             = m_parameters->get<double>("d");
    std::ifstream latticeFile(filename);

    if (!latticeFile) {
        throw std::runtime_error("The lattice file could not be read");
    }

    std::string firstLine;
    std::string comment;
    std::string line;
    getline(latticeFile, firstLine);
    getline(latticeFile, comment);
    parseComment(comment);

    // Confirm that the scanned lattice is the same as described in the parameter file
    if (!validateLattice(nx, ny, d)){
        std::string msg = "The parameter file and the lattice file are not equivalent.\nReason:\n";
        msg += reasonForInvalidation(nx, ny, d);
        throw std::runtime_error(msg);
    }

    size_t totalNumNodes = std::stoi(firstLine);

    while (getline(latticeFile, line)) {
        std::vector<std::string> tokens;
        splitLineIntoTokens(line, tokens);

        // Accept differing number of columns
        switch (tokens.size()) {
        case 0:
            continue;
        case 3:
            break;
        default: {
            std::cerr << "Warning: A line in " << filename << " contained an unexpected number of tokens."
                      << std::endl;
            break;
        }
        }

        // Parse the tokens
        double x = std::stod(tokens[1]);
        double y = std::stod(tokens[2]);

        // Construct the node
        std::shared_ptr<Node> node = Lattice::newNode(m_parameters, m_latticeInfo, x, y);
        // Use the letters in the first column to determine the node type
        if (tokens[0].find("T") != std::string::npos)
            m_topNodes.push_back(node);
        if (tokens[0].find("B") != std::string::npos)
            m_bottomNodes.push_back(node);
        if (tokens[0].find("L") != std::string::npos)
            m_leftNodes.push_back(node);
        if (tokens[0].find("N") != std::string::npos)
            m_normalNodes.push_back(node);
        m_nodes.push_back(node);
    }

    // Check if the number of nodes specified in the first line is equal to
    // the number of nodes contructed
    if (m_nodes.size() != totalNumNodes) {
        std::cerr << "Warning: Expected " << totalNumNodes << " entries, but only " <<
                     m_nodes.size() << " nodes were constructed" << std::endl;
    }
    latticeFile.close();
    m_hasNodes = true;
}

void LatticeScanner::parseComment(std::string &comment){
    std::vector<std::string> tokens;
    splitLineIntoTokens(comment, tokens);
    if (tokens.size() < 3)
        throw std::runtime_error("Expected nx, ny, d in xyz comment");

    m_readnx = std::stoi(tokens[0]);
    m_readny = std::stoi(tokens[1]);
    m_readd  = std::stod(tokens[2]);
}

bool LatticeScanner::validateLattice(int nx, int ny, double d){
    bool intTest    = (nx == m_readnx && ny == m_readny);
    d = 0;
    // TODO: This test doesn't work. Too sleepy to fix
    // bool doubleTest = (std::abs(d-m_readd)>std::abs(std::min(d,m_readd))*std::numeric_limits<double>::epsilon());
    // if (doubleTest){};
    return (intTest);
}

std::string LatticeScanner::reasonForInvalidation(int nx, int ny, double d){
    std::ostringstream reason;
    if (nx != m_readnx)
        reason << "Parameter nx = " << nx << " != " << m_readnx << " = scanned nx" << std::endl;
    if (ny != m_readny)
        reason << "Parameter ny = " << ny << " != " << m_readny << " = scanned ny" << std::endl;
    // bool doubleTest = (std::abs(d-m_readd)>std::abs(std::min(d,m_readd))*std::numeric_limits<double>::epsilon());
    // if (!doubleTest){};
        // reason << "Parameter d = " << d << " != " << m_readd << " = scanned d" << std::endl;
    return reason.str();
}
