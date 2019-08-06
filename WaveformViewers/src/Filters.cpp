//
//  Filters.cpp
//  This is a file which contains a list of functions to run the accumulator on different mathematical objects (i.e. Low Pass filter, Sallen Key)
//
//  Created by Andrew Stevens on 23/11/2018.
//
//If this breaks everything then try:
// -> Acting on a copy of the Pulse.

#include "Filters.hpp"

void lowPassFilter(double& signal, double& accu, const double& exp, const double& oneMinusExp)
{
  accu = exp * accu + oneMinusExp * signal;
  signal = accu;
}

void highPassFilter(double& signal, double& accu, const double& exp, const double& oneMinusExp)
{ 
  accu = exp * accu + oneMinusExp * signal;
  signal -= accu;
}

void poleZero(double& signal, double& accu, const double& exp, const double& tauRatio, const double& oneMinusExp, const double& oneMinusTauRatio)
{
  accu = exp * accu + oneMinusExp * signal;
  signal = tauRatio * signal + oneMinusTauRatio * accu;
}

void sallenKey(double& signal, std::array<double,4>& accus, const double& expCos, const double& expSin, const double& oneMinusExpCos)
{
  double diffR = accus[0]-signal;
  lowPassFilter(signal,accus[0],expCos,oneMinusExpCos);
  accus[0] -= expSin*accus[1];
  accus[1] = expCos*accus[1]+expSin*diffR;

  diffR = accus[2]-accus[0];
  double diffI = accus[3]-accus[1];
  lowPassFilter(accus[0],accus[2],expCos,oneMinusExpCos);
  accus[2] += expSin*diffI;
  lowPassFilter(accus[1],accus[3],expCos,oneMinusExpCos);
  accus[3] -= expSin*diffR;
  
  signal = accus[2];
}
