#include "filteraverage.h"

FilterAverage::FilterAverage(int windowSize)
    : windowSize(windowSize)
{
}

double FilterAverage::processSample(double sample)
{
    buffer.append(sample);
    if (buffer.size() > windowSize)
        buffer.removeFirst();

    double sum = 0;
    for (double val : buffer)
        sum += val;

    return sum / buffer.size();
}
