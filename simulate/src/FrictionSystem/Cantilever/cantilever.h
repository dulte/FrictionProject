#ifndef CANTILEVER_H
#define CANTILEVER_H

#include <memory>
#include "FrictionSystem/frictionsystem.h"

class Parameters;

class Cantilever : public FrictionSystem
{
public:
    Cantilever(std::shared_ptr<Parameters>);
    void startDriving(double) override {};
private:
    int nx;
    int ny;
};


#endif /* CANTILEVER_H */
