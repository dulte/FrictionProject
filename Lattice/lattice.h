#pragma once
#include <memory>
#include <vector>

#include "Node/node.h"
#include "Lattice/simplelattice.h"

class Node;
class LatticeInfo;

class Lattice : public SimpleLattice// , public std::enable_shared_from_this<Lattice>
{
public:
    Lattice();
    ~Lattice();
    std::vector<std::shared_ptr<Node>> bottomNodes;
    std::vector<std::shared_ptr<Node>> topNodes;
    std::vector<std::shared_ptr<Node>> leftNodes;

    virtual void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) = 0;
};

