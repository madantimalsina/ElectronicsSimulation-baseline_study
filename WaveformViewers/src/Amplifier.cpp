//
//  Amplifier.cpp
//  devices
//
//  Created by Cees Carels on 06/07/2016.
//  Copyright © 2016 LZOxford. All rights reserved.
//

#include "Amplifier.hpp"

Amplifier::Amplifier()
{
    /**
    * Constructor for Amplifier.
    */
}

Amplifier::Amplifier(const der::DeviceModel& model)
{
    /**
    * Constructor for Amplifier.
    */
    sModel = model;
    if (sModel == der::DeviceModel::kLowGain)
        doLGSetup();
    else
        doHGSetup();

    accus.fill(0.0);
    skAccus.fill(0.0);
}

Amplifier::~Amplifier()
{
    /**
   * Destructor for Amplifier.
   */
}

void Amplifier::doLGSetup()
{
    double R303(100E+3);
    double R306(100.0E+3);
    double R307(1000.0);
    double R308(499.0);
    double R311(332.0);
    double R314(332.0);
    double R315(215.0);
    double R312(75.0);
    double R321(499.0);

    double C304(0.1E-6);
    double C306(0.1E-6);
    double C308(10.0E-12);
    double C309(10.0E-12);

    double Ra = 1.0 / ((1.0 / R321) + (1.0 / R307) + (1.0 / R312));
    double Rb = (R314 * R315) / (R314 + R315);
    double Rc = R308 * R311 / (R308 + R311);

    double GFAD8099(17000);

    taus[0] = 0.02004963902 * 1e9; //ns
    taus[1] = 0.1491365804;
    taus[2] = Rc * C309 * 1e9;
    taus[3] = R308 * C309 * 1e9;
    taus[4] = Ra * C308 * 1e9;
    taus[5] = 3.3775; //taus 5 & 6 in SK
    taus[6] = 2.3674;

    for(int i = 0; i<5; ++i)
      {
	exps[i] = exp(-fTimeBase/taus[i]);
	oneMinusExps[i] = 1-exps[i]; 
      }

    //For Pole-Zeros
    tauRatios[0] = taus[2]/taus[3];
    tauRatios[1] = taus[4]/taus[1];
    
    for(int i = 0; i<2; ++i)
      {
	oneMinusTauRatios[i] = tauRatios[i];
      }

    //For Sallen-Key
    double tauR = taus[5]/(taus[5]*taus[5]+taus[6]*taus[6]);
    double tauI = taus[6]/(taus[5]*taus[5]+taus[6]*taus[6]);
    double expR = exp(-fTimeBase*tauR);
    exps[5] = expR*cos(-fTimeBase*tauI);
    oneMinusExps[5] = 1-exps[5];
    expRsinI = expR*sin(-fTimeBase*tauI);

    G = (R308 / Rc) * (1.0 / (1.0 + (1.0 / GFAD8099) * (R308 / Rc))) * ((Rb * R307) / (R314 * Ra)) * ((R303 * C304) / (taus[0] * 1e-9));
}

void Amplifier::doHGSetup()
{
    //First stage
    double R405(69.8);
    double R407(499.0);

    double C417(27.0E-12);
    double C418(10.0E-12);

    //Middle stage
    double R413(75.0);
    double R417(499.0);

    double C414(10.0E-12);
    double C415(27.0E-12);

    //Third stage
    double R406(100.0E3);
    double R414(100.0E3);
    double R410(332.0);
    double R411(215.0);
    double R412(1000.0);
    double R416(332.0);
    double R420(215.0);
    double R423(75.0);
    double R427(499.0);

    double Cx(10.0E-12); //Label is C? on older diagram
    double C412(10.0E-12); //this was C408 but we believe it is a typo. 
    double C408(0.1E-6);
    double C409(0.1E-6);
    double Rb3 = R416 * R420 / (R416 + R420);
    double Ra3 = 1.0 / (1.0 / R427 + 1.0 / R412 + 1.0 / R423); //check this expression is correct from the circuit diagram.
    double Rc3 = (R405 * R407) / (R405 + R407);
    double Rd = (R413 * R417) / (R413 + R417);
    double Re = R410 * R411 / (R410 + R411);

    double GFAD8099(17000.0);

    taus[0] = Rd * (C414 + C415) * 1e9;
    taus[1] = 1.13091E-8 * 1e9;
    taus[2] = 5.28874E-9 * 1e9;
    taus[3] = 1.95183E-10 * 1e9;
    taus[4] = Rc3 * (C418 + C417) * 1e9;
    taus[5] = R407 * C418 * 1e9;
    taus[6] = Ra3 * C412 * 1e9;
    taus[7] = 0.1491365804;
    taus[8] = 0.02004963902 * 1e9; //[ns]
    taus[9] = 3.3775; //taus 9 & 10 in SK
    taus[10] = 2.3674;

    for(int i = 0; i<9; ++i)
      {
	exps[i] = exp(-fTimeBase/taus[i]);
	oneMinusExps[i] = 1-exps[i];
      }

    //For Pole-Zeros
    tauRatios[0] = taus[0]/taus[1];
    tauRatios[1] = taus[4]/taus[5];
    tauRatios[2] = taus[6]/taus[7];
    
    for(int i = 0; i<3; ++i)
      {
	oneMinusTauRatios[i] = tauRatios[i];
      }

    //For Sallen-Key
    double tauR = taus[9]/(taus[9]*taus[9]+taus[10]*taus[10]);
    double tauI = taus[10]/(taus[9]*taus[9]+taus[10]*taus[10]);
    double expR = exp(-fTimeBase*tauR);
    exps[9] = expR*cos(-fTimeBase*tauI);
    oneMinusExps[9] = 1-exps[9];
    expRsinI = expR*sin(-fTimeBase*tauI);

    G = (1.0 / (1.0 + (1.0 / GFAD8099) * (R407 / Rc3))) * (Re / R410) * (R407 / Rc3) * (R417 / Rd) * (Rb3 * R412 * R406 * C408) / (R416 * Ra3 * taus[8] * 1e-9);

}

void Amplifier::runFilters(double& sampleValue){ 
  if (sModel == der::DeviceModel::kLowGain)
    runLGFilters(sampleValue);
  else
    runHGFilters(sampleValue);
}

void Amplifier::runHGFilters(double& sampleValue){
  //First part (Similar to LG)
  poleZero(sampleValue,accus[0],exps[5],tauRatios[1],oneMinusExps[5],oneMinusTauRatios[1]);
  //Middle part: SK -> real,LP,LP
  poleZero(sampleValue,accus[1],exps[1],tauRatios[0],oneMinusExps[1],oneMinusTauRatios[0]);
  lowPassFilter(sampleValue,accus[2],exps[2],oneMinusExps[2]);
  lowPassFilter(sampleValue,accus[3],exps[3],oneMinusExps[3]);
  //Last part (same as LG)
  highPassFilter(sampleValue,accus[4],exps[8],oneMinusExps[8]); // applying highPassFilter leads to overshoot
  poleZero(sampleValue,accus[5],exps[7],tauRatios[2],oneMinusExps[7],oneMinusTauRatios[2]);
  sallenKey(sampleValue, skAccus, exps[9], expRsinI, oneMinusExps[9]);
  sampleValue *= G/2.0; //Here the factor of 2 is to account for the potential divider element that comes after the Amplifier but before the Digitiser. 
}

void Amplifier::runLGFilters(double& sampleValue){
  highPassFilter(sampleValue,accus[0],exps[0],oneMinusExps[0]);
  poleZero(sampleValue,accus[1],exps[3],tauRatios[1],oneMinusExps[3],oneMinusTauRatios[1]);
  poleZero(sampleValue,accus[2],exps[1],tauRatios[0],oneMinusExps[1],oneMinusTauRatios[0]);
  sallenKey(sampleValue, skAccus, exps[5], expRsinI, oneMinusExps[5]);
  sampleValue *= G/2.0; //See HG case to explain factor of 2. 
}

void Amplifier::reset(){
  accus.fill(0.0);
  skAccus.fill(0.0);
}
