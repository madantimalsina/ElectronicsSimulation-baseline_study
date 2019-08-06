//
//  FFT.hpp
//  devices
//
//  Created by Cees Carels on 02/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#ifndef FFT_hpp
#define FFT_hpp

#include <stdio.h>
#include <string>

#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMath.h"

#include "DBInterface.hpp"
#include "Pulse.hpp"

/**
 * Class to provide FFT of pulse data.
 *
 * The class methods were adapted from OxRop/Software7.3 to be usable with DER.
 */

class FFT
{
public:
    FFT();
    FFT(const double TBase);
    ~FFT();

    void doRealFFT(Pulse& thePulse);
    void PlotFFT(const std::string& name,
        const std::string& evt,
        const std::string& channel,
        const std::string& pulseID);

private:
    void RealFFT(std::vector<double>& data, Bool_t forward);
    void ToPwr2(std::vector<double>& data);
    void BitReversal(std::vector<double>& data);
    void DLLemma(std::vector<double>& data, const Bool_t invert);
    void Fourier(std::vector<double>& data, Bool_t forward);

    double* RawDataPtr;
    unsigned long* RawDataSize;
    double* freqPtr;
    double* respPtr;

    double TimeBase;
};

#endif /* FFT_hpp */
