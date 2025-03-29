#include "filtermedian.h"
#include <algorithm>

FilterMedian::FilterMedian(int windowSize)
    : windowSize(windowSize)
{
}

double FilterMedian::processSample(double sample)
{
    window.append(sample);
    if (window.size() > windowSize)
        window.removeFirst();

    QVector<double> sorted = window;
    std::sort(sorted.begin(), sorted.end());
    return sorted[sorted.size() / 2];
}
