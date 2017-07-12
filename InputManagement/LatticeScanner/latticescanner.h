#pragma once
#include <fstream>
#include <vector>
#include <memory>

class Node;
class Parameters;
class LatticeInfo;

class LatticeScanner
{
public:
    explicit LatticeScanner(std::shared_ptr<Parameters>,
                            std::shared_ptr<LatticeInfo>);
    virtual ~LatticeScanner();

    // Scans the file (xyz-format) and constructs the nodes
    // TODO: Move scan into the constructor?
    void scan();
    void splitLineIntoTokens(std::string &s, std::vector<std::string> &tokens);
    // Extracts nx, ny and d from the comment string
    void parseComment(std::string &comment);
    bool hasNodes(){return m_hasNodes;};
    // Returns true if the scanned lattice has the same
    // dimensions and distance d as the parameters in
    // the parameter file
    bool validateLattice(int nx, int ny, double d);
    // Returns the reason for failure of validateLattice
    std::string reasonForInvalidation(int, int, double);

    std::vector<std::shared_ptr<Node>> m_nodes;
    std::vector<std::shared_ptr<Node>> m_bottomNodes;
    std::vector<std::shared_ptr<Node>> m_topNodes;
    std::vector<std::shared_ptr<Node>> m_leftNodes;
    std::vector<std::shared_ptr<Node>> m_normalNodes;
private:
    std::shared_ptr<Parameters>  m_parameters;
    std::shared_ptr<LatticeInfo> m_latticeInfo;
    bool                         m_hasNodes = false;
    int                          m_readnx;
    int                          m_readny;
    double                       m_readd;
};
