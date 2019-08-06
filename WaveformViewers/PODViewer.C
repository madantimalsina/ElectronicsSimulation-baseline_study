#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TString.h"

#include "PODGui.C"

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>

TCanvas* gPodCanvas = 0;

//---------------------------------------------------------------------
TMultiGraph* pod_stage_plots(Data* datum, StageData* stageDatum, Int_t& maxSample, Int_t& minSample)
{
  TString mgname = Form("POD %d", gPodID);
  TString mgtitle = Form("POD %d", gPodID);
  TMultiGraph* mg = new TMultiGraph(mgname, mgtitle);

  std::string names[6] = {"Total", "PMT", "PMTCable", "Amp", "FTCable","Trig"};  
  std::vector<std::vector<Short_t>* > data;
  data.push_back(datum->zData);
  data.push_back(stageDatum->pmtData);
  data.push_back(stageDatum->pmtCableData);
  data.push_back(stageDatum->ampData);
  data.push_back(stageDatum->feedthroughCableData);
  data.push_back(stageDatum->trigData);
  for(int i = 0; i<6; ++i){
    Double_t * xaxis = new Double_t[datum->zData->size()];
    Double_t * samps = new Double_t[datum->zData->size()];
    Int_t nSamps = data[i]->size();
    if(nSamps>0){
      for (Int_t j=0; j<nSamps; ++j){
	xaxis[j] = j;
	samps[j] = data[i]->at(j); 
	if(samps[j]>maxSample) maxSample = samps[j];
	else if (samps[j]<minSample) minSample = samps[j];
      }
      TGraph* gr = new TGraph(nSamps,xaxis,samps);
      TString grName = Form("POD %d - %s", gPodID, names[i].c_str());
      gr->SetName(grName);
      gr->SetTitle(grName);
      gr->GetYaxis()->SetTitle("ADC Counts");
      gr->GetXaxis()->SetTitle("Samples");
      gr->GetYaxis()->SetTitleOffset(1.5);
      gr->GetXaxis()->SetTitleOffset(1.3);
      gr->SetLineColor(i+1);
      if(i>=4) gr->SetLineColor(i+2);
      mg->Add(gr);
    }
  }

  return mg;
}

//---------------------------------------------------------------------
void load_event()
{
  printf("Loading pod %d.\n", gPodID);

  // FOR DEFAULT SORTED EVENTS:
  Long64_t local = gPodID;
  gData->GetEntry(local);
  gStageData->GetEntry(local);

  gEventLabel->SetText(Form("%d", gData->evt));
  gChannelLabel->SetText(Form("%d", gData->channel));
  gPodCanvas->Clear();
  gPodCanvas->SetName("POD");
  gPodCanvas->SetTitle(Form("POD %d", gPodID));
  gPodCanvas->cd(0);
 
  Int_t minSample = 7372;
  Int_t maxSample = 7372;
  TMultiGraph* gr_pods = pod_stage_plots(gData,gStageData,minSample,maxSample);
  gr_pods->Draw("al");
  gr_pods->GetYaxis()->SetTitle("ADC Counts");
  gr_pods->GetXaxis()->SetTitle("Samples");
  gr_pods->GetYaxis()->SetTitleOffset(1.5);
  gr_pods->GetXaxis()->SetTitleOffset(1.3);
  gr_pods->GetYaxis()->SetRangeUser(minSample,maxSample);
  gPodCanvas->Update();
}

void PODViewer() {

  TString filename;
  bool fileFound = false;
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
  TTree* t_data = (TTree*)   f->Get("Data");
  TTree* t_stageData = (TTree*) f->Get("StageData");

  if(t_stageData->GetEntries()==0){
    std::cout << "File does not contain stage data: DER must be run with GenerateStageData -> true" << std::endl;
    exit(1);
  }
  else{
    gData = new Data(t_data);
    gStageData = new StageData(t_stageData);

    TEveManager::Create();

    make_gui();

    gEve->GetBrowser()->StartEmbedding(1);
    gPodCanvas = new TCanvas();
    gEve->GetBrowser()->StopEmbedding("POD");

    gEve->GetBrowser()->SetTab(1, 1);

    load_event();
  }
}
