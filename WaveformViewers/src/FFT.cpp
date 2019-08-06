//
//  FFT.cpp
//  devices
//
//  Created by Cees Carels on 02/08/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "FFT.hpp"

FFT::FFT()
{
    /**
     * Constructor for FFT.
     */
}

FFT::FFT(const double TBase)
    : TimeBase(TBase)
{
    /**
     * Constructor for FFT.
     */
}

FFT::~FFT()
{
    /**
     * Destructor for FFT
     */

    /*delete RawDataPtr;
    delete RawDataSize;
    delete freqPtr;
    delete respPtr;*/
}

void FFT::ToPwr2(std::vector<double>& data)
{
    /**
     * Make sure that the array is a length which is a power of 2
     * Currently just truncates, later add options to pad also.
     */

    unsigned long size = data.size();
    int size2 = 1;
    int power = 0;
    while (size2 < size)
    {
        size2 *= 2;
        ++power;
    }
    if (size2 != size)
    {
        --power;
        data.resize(size2 / 2);
    }
}

void FFT::BitReversal(std::vector<double>& data)
{
    /**
     * Bit reversal.
     */
    unsigned long size = data.size();
    int j = 1;
    int m = -1; // initialised properly later

    for (int i = 1; i < size; i += 2)
    {
        if (j > i)
        {
            double temp1 = data[j - 1];
            data[j - 1] = data[i - 1];
            data[i - 1] = temp1;
            double temp2 = data[j];
            data[j] = data[i];
            data[i] = temp2;
        }
        m = size / 2;
        while ((m >= 2) && (j > m))
        {
            j -= m;
            m /= 2;
        }
        j += m;
    }
}

void FFT::DLLemma(std::vector<double>& data, const Bool_t invert)
{
    /**
     * Daniel-Lanczos Lemma.
     */
    int iStep;
    int mMax = 2;
    unsigned long size = data.size();

    double theta;
    double tempW;
    double reW;
    double imW;
    double reWp;
    double imWp;

    double reTemp;
    double imTemp;

    while (size > mMax)
    {
        iStep = mMax * 2;
        theta = 2.0 * TMath::Pi() / mMax;
        if (!invert)
            theta *= -1.0;
        tempW = TMath::Sin(0.5 * theta);
        reWp = -2.0 * tempW * tempW;
        imWp = TMath::Sin(theta);
        reW = 1.0;
        imW = 0.0;

        for (int m = 1; m < mMax; m += 2)
        {
            for (int i = m; i <= size; i += iStep)
            {
                int j = i + mMax;
                reTemp = reW * data[j - 1] - imW * data[j];
                imTemp = reW * data[j] + imW * data[j - 1];
                data[j - 1] = data[i - 1] - reTemp;
                data[j] = data[i] - imTemp;
                data[i - 1] += reTemp;
                data[i] += imTemp;
            }
            tempW = reW;
            reW = (reW * reWp) - (imW * imWp) + reW;
            imW = (imW * reWp) + (tempW * imWp) + imW;
        }
        mMax = iStep;
    }
    if (invert)
        for (int i = 0; i < size; i++)
            data[i] /= double(size / 2);
}

void FFT::Fourier(std::vector<double>& data, Bool_t forward = true)
{
    /**
     * Do Fourier transform.
     */
    unsigned long size = data.size();

    int j(1);
    for (int i = 1; i < size; i += 2)
    {
        if (j > i)
        {
            double temp = data[j - 1];
            data[j - 1] = data[i - 1];
            data[i - 1] = temp;
            temp = data[j];
            data[j] = data[i];
            data[i] = temp;
        }
        unsigned long k = size >> 1;
        while ((1 < k) && (k < j))
        {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    int qlim(2);
    while (qlim < size)
    {
        double th = (forward) ? TMath::Pi() / qlim : -TMath::Pi() / qlim;
        int qlimTwice = qlim << 1;
        for (int q = 0; q < qlim; q += 2)
        {
            double fCos = TMath::Cos(q * th);
            double fSin = TMath::Sin(q * th);
            for (int i = q; i < size; i += qlimTwice)
            {
                double Sr = fCos * data[qlim + i] - fSin * data[qlim + i + 1];
                double Si = fCos * data[qlim + i + 1] + fSin * data[qlim + i];
                data[qlim + i] = data[i] - Sr;
                data[qlim + i + 1] = data[i + 1] - Si;
                data[i] += Sr;
                data[i + 1] += Si;
            }
        }
        qlim = qlimTwice;
    }
}

void FFT::RealFFT(std::vector<double>& data, Bool_t forward)
{
    /**
     * RealFFT of data.
     */
    unsigned long size = data.size();
    std::cout << "size " << size << std::endl;
    double k1(0.5), k2(0.5), th(-TMath::Pi() / size);
    if (forward)
    {
        Fourier(data);
        k2 = -k2;
        th = -th;
    }

    for (int n = 2; n < (size >> 1); n += 2)
    {
        unsigned long p = size - n;
        double fCos = TMath::Cos(n * th);
        double fSin = TMath::Sin(n * th);

        double fAr = k1 * (data[n] + data[p]);
        double fAi = k1 * (data[n + 1] - data[p + 1]);
        double fBr = -k2 * (data[n + 1] + data[p + 1]);
        double fBi = k2 * (data[n] - data[p]);
        data[n] = fAr + fCos * fBr - fSin * fBi;
        data[n + 1] = fAi + fCos * fBi + fSin * fBr;
        data[p] = fAr - fCos * fBr + fSin * fBi;
        data[p + 1] = -fAi + fCos * fBi + fSin * fBr;
    }

    double temp = data[0];
    data[0] = temp + data[1];
    data[1] = temp - data[1];

    if (!forward)
    {
        data[0] *= k1;
        data[1] *= k1;
        Fourier(data, kFALSE);
    }
}

void FFT::doRealFFT(Pulse& thePulse)
{
    /**
     * Do RealFFT based on pulse object.
     */

    unsigned long N = thePulse.size();

    std::vector<double> x_arr;
    x_arr.resize(N);

    for (int i = 0; i < N; i++)
        x_arr[i] = thePulse[i];

    //Windowing
    double inc = 2 * TMath::Pi() / double(N - 1);
    for (int i = 0; i < N; ++i)
        x_arr[i] *= 1 - TMath::Cos(i * inc);

    RealFFT(x_arr, kFALSE);

    double freqBase(0);
    TArrayD resA;
    TArrayD frqA;
    TArrayD data;
    int nSet(1);
    unsigned long int fL = N / 2 - 1;
    freqBase = 1.0 / (TimeBase * N);

    double* res = new double[N];
    for (int i = 2; i < N; i += 2)
    {
        double fR2 = x_arr[i] * x_arr[i];
        double fI2 = x_arr[i + 1] * x_arr[i + 1];
        double fftResult = TimeBase * sqrt(2 * (fR2 + fI2) * freqBase);
        res[i / 2 - 1] += fftResult;
    }

    double* frq = new double[N / 2];
    for (int i = 0; i < fL; ++i)
    {
        frq[i] = (i + 1) * freqBase;
        res[i] /= nSet;
    }

    TCanvas* c1 = new TCanvas("Canvas", "", 400, 400, 800, 800);
    TGraph* gr1 = new TGraph((Int_t)fL, frq, res);
    c1->SetGrid();
    c1->SetLogx();
    c1->SetLogy();

    std::string title = "Fourier Transform";
    gr1->SetTitle(title.c_str());
    gr1->GetXaxis()->SetTitle("Frequency [Hz]");
    gr1->GetYaxis()->SetTitle("Amplitude [V Hz^{-2}]");
    gr1->GetXaxis()->SetRangeUser(1E-3, 1E12);
    gr1->GetYaxis()->SetRangeUser(1E-20, 1E10);

    gr1->GetXaxis()->SetTitleOffset(1.3);
    gr1->GetYaxis()->SetTitleOffset(1.56);

    gr1->GetXaxis()->SetLabelSize(0.025);
    gr1->GetYaxis()->SetLabelSize(0.025);

    gr1->GetXaxis()->SetTitleSize(0.03);
    gr1->GetYaxis()->SetTitleSize(0.03);

    gr1->SetMarkerStyle(21);
    gr1->SetLineWidth(1);
    gr1->SetLineColor(1);

    gr1->Draw("AL");

    std::string dir = "~/Desktop/";
    std::string file = "FFT";
    std::string ext = ".pdf";
    std::string outputpath = dir + file + ext;

    c1->SaveAs(outputpath.c_str());

    delete[] res;
}

void FFT::PlotFFT(const std::string& name,
    const std::string& evt,
    const std::string& channel,
    const std::string& pulseID)
{
    /**
     * Method currently left empty, will be implemented. Will provide
     * plotting of FFT.
     */
}
