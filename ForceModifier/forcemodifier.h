#pragma once
#include "Vec3/vec3.h"
#include <memory>


class Node;

class ForceModifier
{
public:
    ForceModifier();

    virtual vec3 getForceModification()  {return vec3();}
    virtual double getMomentModification() {return 0;}
    virtual void setNode(std::shared_ptr<Node> node) {m_node = node;}
    virtual void initialize() {;}
    //virtual void fileOutputAction(std::shared_ptr<H5::H5File>) {;}

protected:
    std::shared_ptr<Node> m_node;
};
