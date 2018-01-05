#ifndef BULKSTRETCH_H
#define BULKSTRETCH_H

#include <memory>
#include "FrictionSystem/frictionsystem.h"

class Parameters;

class BulkStretch : public FrictionSystem
{
public:
    BulkStretch(std::shared_ptr<Parameters>);
    void startDriving(double) override {};
    void postProcessing() override;
private:
    int nx;
    int ny;
    double initialxLength;
    double initialyLength;
    double force;
    double hZ;
};

#endif /* BULKSTRETCH_H */
