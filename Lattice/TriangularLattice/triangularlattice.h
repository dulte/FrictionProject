#ifndef TRIANGULARLATTICE_H
#define TRIANGULARLATTICE_H

#include <vector>
#include "Lattice/lattice.h"
#include "Node/node.h"
#include "LatticeInfo/latticeinfo.h"

class LatticeInfo;
class Parameters;

class TriangularLattice : public Lattice
{
public:
    TriangularLattice();

    void populate(std::shared_ptr<Parameters>) override;
    void populate(std::shared_ptr<Parameters>, int, int) override;
    void populateWithUnitCell(std::shared_ptr<Parameters>);
    void populateCantilever(std::shared_ptr<Parameters>);
 private:
    int m_nx;
    int m_ny;
};

#endif // TRIANGULARLATTICE_H
