#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TString.h"

#include "RawGui.C"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>

TCanvas* gEventCanvas = 0;

//---------------------------------------------------------------------
TGraph* raw_plot(RawData* rawDatum, bool isHG)
{
  std::vector<double>* rawDataPoints = (!isHG ? rawDatum->rawLGData 
					 : rawDatum->rawHGData);

  Double_t * xaxis = new Double_t[rawDataPoints->size()];
  Double_t * samps = new Double_t[rawDataPoints->size()];
  for(size_t i = 0; i<rawDataPoints->size(); ++i){
    xaxis[i] = i;
    samps[i] = rawDataPoints->at(i);
  }

  TGraph* gr = new TGraph(rawDataPoints->size(),xaxis,samps);
  TString gain = (!isHG ? "LG" :  "HG");
  TString grName = Form("Event %d#; PMT %d - ", rawDatum->evt, rawDatum->pmt);
  grName += gain;
  gr->SetName(gain);
  gr->SetTitle(grName);
  gr->GetYaxis()->SetTitle("ADC Counts");
  gr->GetXaxis()->SetTitle("Samples");
  gr->GetXaxis()->SetRangeUser(0,rawDataPoints->size());
  gr->GetYaxis()->SetTitleOffset(1.5);
  gr->GetXaxis()->SetTitleOffset(1.3);
  
  return gr;
}

std::pair<TGraph*,TGraph*> photon_boundary_plots(RawData* rawDatum, bool isHG)
{
  std::vector<int>* photonStartPoints = (!isHG ? rawDatum->photonLGStarts 
					 : rawDatum->photonHGStarts);
  std::vector<int>* photonEndPoints = (!isHG ? rawDatum->photonLGEnds 
					 : rawDatum->photonHGEnds);

  Int_t * xStartsAxis = new Int_t[photonStartPoints->size()*2];
  Int_t * xEndsAxis = new Int_t[photonStartPoints->size()*2];
  Int_t * samps = new Int_t[photonStartPoints->size()*2];
  for(size_t i = 0; i<photonStartPoints->size(); ++i){
    xStartsAxis[i] = photonStartPoints->at(i);
    xEndsAxis[i] = photonEndPoints->at(i);
    samps[i] = 7372;
  }

  TGraph* grStarts = new TGraph(photonStartPoints->size(),xStartsAxis,samps);
  TGraph* grEnds = new TGraph(photonEndPoints->size(),xEndsAxis,samps);
  grStarts->SetMarkerStyle(2);
  grEnds->SetMarkerStyle(5);
  grStarts->SetMarkerColor(kRed);
  grEnds->SetMarkerColor(kRed);

  return std::make_pair(grStarts,grEnds);
}

TGraph* pod_boundary_plots(RawData* rawDatum, bool isHG)
{
  std::vector<int>* podStartPoints = (!isHG ? rawDatum->podLGStarts 
					 : rawDatum->podHGStarts);
  std::vector<int>* podEndPoints = (!isHG ? rawDatum->podLGEnds 
					 : rawDatum->podHGEnds);

  Int_t * xaxis = new Int_t[podStartPoints->size()*2];
  Int_t * samps = new Int_t[podStartPoints->size()*2];
  for(size_t i = 0; i<podStartPoints->size(); ++i){
    xaxis[i*2] = podStartPoints->at(i);
    xaxis[i*2+1] = podEndPoints->at(i);
    samps[i*2] = samps[i*2+1] = 7372;
  }

  TGraph* gr = new TGraph(podStartPoints->size()*2,xaxis,samps);
  gr->SetMarkerStyle(3);
  gr->SetMarkerColor(kBlue);

  return gr;
}


//---------------------------------------------------------------------
void load_event()
{
  printf("Loading event %d.\n", gEventID);

  // FOR DEFAULT SORTED EVENTS:
  Long64_t local = gEventID;
  gRawData->GetEntry(local);
  gEventLabel->SetText(Form("%d", gRawData->evt));
  gPmtLabel->SetText(Form("%d", gRawData->pmt));
  gEventCanvas->Clear();
  gEventCanvas->SetName("Event");
  gEventCanvas->SetTitle(Form("Event %d; ", gEventID));
  gEventCanvas->Divide(1,2);

  gEventCanvas->cd(1);
  TGraph* hgGraph = raw_plot(gRawData,1);
  hgGraph->Draw("al");
  // TGraph* photonHGGraph = pulse_boundary_plots(gRawData,1);
  // photonHGGraph->Draw("psame");
  std::pair<TGraph*,TGraph*> photonHGGraphs = photon_boundary_plots(gRawData,1);
  photonHGGraphs.first->Draw("psame");
  photonHGGraphs.second->Draw("psame");
  TGraph* podHGGraph = pod_boundary_plots(gRawData,1);
  podHGGraph->Draw("psame");

  gEventCanvas->cd(2);
  TGraph* lgGraph = raw_plot(gRawData,0);	    
  lgGraph->Draw("al");
  std::pair<TGraph*,TGraph*> photonLGGraphs = photon_boundary_plots(gRawData,0);
  photonLGGraphs.first->Draw("psame");
  photonLGGraphs.second->Draw("psame");
  TGraph* podLGGraph = pod_boundary_plots(gRawData,0);
  podLGGraph->Draw("psame");

  gEventCanvas->cd(0);
  gEventCanvas->Update();
}

void RawViewer() {

  TString filename;
  std::cout << "Specify file name: " << std::endl;
  std::cin >> filename;
  std::ifstream fileTest(((string)filename).c_str());
  while(!filename.Contains(".root") || !fileTest.good()){
    std::cout << "File doesn't exist or doesn't end in .root - please specify a different file: " << std::endl;
    std::cin >> filename;
    fileTest.open(filename);
  }
  fileTest.close();

  TFile* f = TFile::Open(filename);
  std::cout << "Opened " << filename << std::endl;
  TTree* t_rawData = (TTree*)f->Get("RawData");

  if(t_rawData->GetEntries()==0){
    std::cout << "File does not contain raw data: DER must be run with WriteRawData -> true" << std::endl;
    exit(1);
  }
  else{
    gRawData = new RawData(t_rawData);

    TEveManager::Create();

    make_gui();

    gEve->GetBrowser()->StartEmbedding(1);
    gEventCanvas = new TCanvas();
    gEve->GetBrowser()->StopEmbedding("Event");

    gEve->GetBrowser()->SetTab(1, 1);

    load_event();
  }
}
