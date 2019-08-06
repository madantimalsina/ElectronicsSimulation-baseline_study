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

void POD_baseline_finding(){

    TFile* f = TFile::Open("lz_201704030145_000010_000650_raw.root");

    TFile*fout = new TFile("BaseLine.root","RECREATE");

    TH1F* pre_baseline_h = new TH1F("Prebaseline_h","PREbaseline_h",1000,7000,8000);
    TH1F* post_baseline_h = new TH1F("POSTbaseline_h","POSTbaseline_h",1000,7000,8000);

    TH2F* pre_baseline_var_h = new TH2F("pre_baseline_var","pre_baseline_var",1000,7000,8000,1000,0,1000);
    TH2F* post_baseline_var_h = new TH2F("post_baseline_var","post_baseline_var",1000,7000,8000,1000,0,1000);
    TTree* data = (TTree*) f->Get("Data");
    std::vector<Short_t> *samples = 0;
    UShort_t event=0,channel =0;
    data->SetBranchAddress("zData",&samples);
    data->SetBranchAddress("evt",&event);
    data->SetBranchAddress("channel",&channel);

    for (int i=0;i<data->GetEntries();i++){
        data->GetEntry(i);
        if (i%10000 == 0) cout<<" Processing "<<i<<" events "<<endl;

        double pre_samples = 0;
        double post_samples = 0;
        double pre_sampele_variance = 0;
        double post_sample_variance = 0;
        for (int j=0;j<samples->size();j++){


            if (j<10){
                pre_samples += samples->at(j);
                pre_sampele_variance += samples->at(j)*samples->at(j);
            }

            if (j>samples->size()-11){
                //cout<<" POD No. "<<i<<" Sample No. "<<j<<" is : "<<samples->at(j)<<endl;
                post_samples += samples->at(j);
                post_sample_variance += samples->at(j)*samples->at(j);
            }


        }
        //cout<<"Before average :  Ped : "<<pre_samples<<" PED VAR : "<<pre_sampele_variance/10<<" Pre^2 : "<<pre_samples*pre_samples<<endl;
        pre_samples /= 10;
        pre_sampele_variance = pre_sampele_variance/10 - pre_samples*pre_samples;
        //if (pre_sampele_variance>100)
        //    cout<<" Ped : "<<pre_samples<<" PED VAR : "<<pre_sampele_variance<<" POD : "<<i<<" event : "<<event<<" channel : "<<channel<<endl;
        pre_baseline_h->Fill(pre_samples);
        pre_baseline_var_h->Fill(pre_samples,pre_sampele_variance);
        //cout<<"Before average : Post Ped : "<<post_samples<<" PED VAR : "<<post_sample_variance/10<<" Pre^2 : "<<post_samples*post_samples<<endl;
        post_samples /= 10;
        post_sample_variance = post_sample_variance/10 - post_samples*post_samples;
        //if (post_sample_variance<100&&post_sample_variance>5&&post_samples<7300)
        //    cout<<" POST Ped : "<<post_samples<<" PED VAR : "<<post_sample_variance<<" POD : "<<i<<" event : "<<event<<" channel : "<<channel<<endl;
        //cout<<"Post Ped : "<<post_samples<<" PED VAR : "<<post_sample_variance<<endl;
        post_baseline_h->Fill(post_samples);
        post_baseline_var_h->Fill(post_samples,post_sample_variance);
        //getchar();
        for (int j=0;j<samples->size();j++){
            double amp = samples->at(j) - pre_samples;
            if (amp > 10 && post_sample_variance > 10){
                cout<<" Ped : "<<pre_samples<<" PED VAR : "<<pre_sampele_variance<<" Large POD ID : "<<i<<" event : "<<event<<" channel : "<<channel<<endl;
                break;
            }


        }

    }
    fout->Write();
    fout->Close();

}
