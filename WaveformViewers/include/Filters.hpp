//
//  Filters.hpp
//  This is a file which contains a list of functions to run the accumulator on different mathematical objects (i.e. Low Pass filter, Sallen Key)
//
//  Created by Andrew Stevens on 23/11/2018.
//
//If this breaks everything then try:
// -> Acting on a copy of the Pulse.

#include "Pulse.hpp"
#include <complex>

void lowPassFilter(double& signal, double& accu, const double& exp, const double& oneMinusExp);

void highPassFilter(double& signal, double& accu, const double& exp, const double& oneMinusExp);

void poleZero(double& signal, double& accu, const double& exp, const double& tauRatio, const double& oneMinusExp, const double& oneMinusTauRatio);

void sallenKey(double& signal, std::array<double,4>& accus, const double& expCos, const double& expSin, const double& oneMinusExpCos);
