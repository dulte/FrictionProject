#ifndef SQUARELATTICE_H
#define SQUARELATTICE_H

#include "Lattice/lattice.h"

class SquareLattice : public Lattice
{
public:
    SquareLattice();
    void populate(int nx, int ny, double d, double E, double nu, double hZ, double density) override;
};

#endif // SQUARELATTICE_H
