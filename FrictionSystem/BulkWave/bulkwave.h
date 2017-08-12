#ifndef BULKWAVE_H
#define BULKWAVE_H

#include <memory>
#include "FrictionSystem/frictionsystem.h"

class Parameters;

class BulkWave : public FrictionSystem
{
public:
    BulkWave(std::shared_ptr<Parameters>);
    void startDriving(double) override {};
};


#endif /* BULKWAVE_H */
