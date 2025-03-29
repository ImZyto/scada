#ifndef FILTERMEDIAN_H
#define FILTERMEDIAN_H

#include "dataprocessor.h"
#include <QVector>

class FilterMedian : public DataProcessor
{
public:
    FilterMedian(int windowSize = 5);

    double processSample(double sample) override;

private:
    QVector<double> window;
    int windowSize;
};

#endif // FILTERMEDIAN_H
