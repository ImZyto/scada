#ifndef FILTERSMOOTH_H
#define FILTERSMOOTH_H

#include "dataprocessor.h"

class FilterSmooth : public DataProcessor
{
public:
    FilterSmooth(double alpha = 0.2);

    double processSample(double sample) override;

private:
    double alpha;
    double previousValue;
    bool hasPrev = false;
};

#endif // FILTERSMOOTH_H
