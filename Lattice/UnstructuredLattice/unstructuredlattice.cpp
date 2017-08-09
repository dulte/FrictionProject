#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "NodeInfo/nodeinfo.h"
#include "unstructuredlattice.h"
#include "InputManagement/Parameters/parameters.h"
#include "InputManagement/LatticeScanner/latticescanner.h"


void UnstructuredLattice::populate(std::shared_ptr<Parameters> parameters){
    m_nx = parameters->get<int>("nx");
    m_ny = parameters->get<int>("ny");
    m_d  = parameters->get<double>("d");
    latticeInfo = latticeInfoFromParameters(parameters);

    LatticeScanner scanner = LatticeScanner(parameters,
                                            latticeInfo);

    scanner.scan();
    // Confirm that the scanner has indeed scanned a lattice
    if (!scanner.hasNodes())
        throw std::runtime_error("Tried to populate from empty scanner");

    // Construct the lattice
    bottomNodes = scanner.m_bottomNodes;
    topNodes    = scanner.m_topNodes;
    leftNodes   = scanner.m_leftNodes;
    normalNodes = scanner.m_normalNodes;
    nodes       = scanner.m_nodes;
    connectNodes();
}

void UnstructuredLattice::connectNodes(){
    for (auto & node : nodes)
    {
        node->setLattice(shared_from_this());
        for (auto & node2: nodes)
        {
            if (node->distanceTo(*node2) < m_d*1.01 && node->distanceTo(*node2) > m_d*0.01)
                node->connectToNode(node2);
        }
    }
}

void UnstructuredLattice::populate(std::shared_ptr<Parameters> parameters, int nx, int ny){
    std::cout << *parameters << nx << ny;
    throw std::runtime_error("Not implemented");
}
