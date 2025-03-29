#include "filtersmooth.h"

FilterSmooth::FilterSmooth(double alpha)
    : alpha(alpha)
{
}

double FilterSmooth::processSample(double sample)
{
    if (!hasPrev) {
        previousValue = sample;
        hasPrev = true;
    } else {
        previousValue = alpha * sample + (1 - alpha) * previousValue;
    }
    return previousValue;
}
