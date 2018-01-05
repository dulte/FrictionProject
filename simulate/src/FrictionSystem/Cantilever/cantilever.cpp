#include <memory>
#include "cantilever.h"
#include "InputManagement/Parameters/parameters.h"
#include "Lattice/TriangularLattice/triangularlattice.h"
#include "ForceModifier/ConstantForce/constantforce.h"
#include "ForceModifier/RelativeVelocityDamper/relativevelocitydamper.h"

Cantilever::Cantilever(std::shared_ptr<Parameters> parameters)
    : FrictionSystem(parameters)
{
    m_lattice = std::make_shared<TriangularLattice>();
    m_lattice->populateCantilever(parameters);

    std::shared_ptr<ConstantForce> myForce = std::make_shared<ConstantForce>(vec3(0, -200, 0));
    m_lattice->nodes[1]->addModifier(std::move(myForce));
    std::shared_ptr<RelativeVelocityDamper> myModifier = std::make_shared<RelativeVelocityDamper>(10);
    m_lattice->nodes[1]->addModifier(std::move(myModifier));

    m_lattice->nodes[0]->isSetForce(true);
}
