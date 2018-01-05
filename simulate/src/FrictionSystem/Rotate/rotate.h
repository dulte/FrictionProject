#ifndef ROTATE_H
#define ROTATE_H

#include <memory>
#include "FrictionSystem/frictionsystem.h"

class Parameters;
class Rotate: public FrictionSystem
{
public:
    Rotate(std::shared_ptr<Parameters>);
    virtual ~Rotate(){};
    void startDriving(double) override {};
    void postProcessing() override {};
private:
};


#endif /* ROTATE_H */
