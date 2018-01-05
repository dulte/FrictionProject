#pragma once
#include "ForceModifier/forcemodifier.h"
#include "InputManagement/Parameters/parameters.h"
#include "Node/node.h"
#include "DriverBeam/driverbeam.h"
#include "Vec3/vec3.h"
#include "memory"
#include <vector>


class StraightenerForce: public ForceModifier
{
public:
    StraightenerForce(std::vector<std::shared_ptr<Node>> nodes,std::shared_ptr<Parameters>  spParameters);
    vec3 getForceModification() override;

protected:
    double m_k = 0;
    std::vector<std::shared_ptr<Node> > m_nodes;
    int m_nx;


};


