#ifndef FILTERAVERAGE_H
#define FILTERAVERAGE_H

#include "dataprocessor.h"

class FilterAverage : public DataProcessor
{
public:
    FilterAverage(int windowSize = 5);

    double processSample(double sample) override;

private:
    QVector<double> buffer;
    int windowSize;
};

#endif // FILTERAVERAGE_H
