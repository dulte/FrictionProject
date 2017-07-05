#pragma once
#include <memory>
#include <vector>

#include "Node/node.h"
#include "Lattice/simplelattice.h"

class Node;
class LatticeInfo;
class Parameters;

class Lattice : public SimpleLattice// , public std::enable_shared_from_this<Lattice>
{
public:
    Lattice();
    ~Lattice();
    std::vector<std::shared_ptr<Node>> bottomNodes;
    std::vector<std::shared_ptr<Node>> topNodes;
    std::vector<std::shared_ptr<Node>> leftNodes;

    virtual void populate(std::shared_ptr<Parameters> parameters) = 0;
    static std::shared_ptr<LatticeInfo> latticeInfoFromParameters(std::shared_ptr<Parameters> parameters);
    static std::shared_ptr<Node> newNode(std::shared_ptr<Parameters>, std::shared_ptr<LatticeInfo>,
                                         double x, double y);
    virtual void step(double dt) override;
};

