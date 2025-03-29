#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QVector>

class DataProcessor
{
public:
    virtual ~DataProcessor() = default;
    virtual double processSample(double sample) = 0;
};

#endif // DATAPROCESSOR_H
