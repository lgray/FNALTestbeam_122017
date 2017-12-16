// include std libraries                                                                        
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <string.h>
#include <sstream>
// include ROOT libraries 
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TProfile.h"

#define MAX_TOFPET_CHANNEL 64
using namespace std;

int main(int argc, char* argv[]){
   if(argc < 2) {
    cerr << "Please give 2 arguments " << "inputFileName " << " " << "outputFileName" <<endl;
    return -1;
  }
  const char *inputFileName = argv[1];
  const char *outFileName   = argv[2];
  TFile *AnaFile=new TFile(inputFileName,"read");
  TTree *anatree = (TTree*)AnaFile->Get("data");
  
  TFile *outFile = new TFile(outFileName,"recreate");
  TTree *outTree = new TTree("data","data");
  outTree->SetAutoSave();

  //=====================Initialize input tree variables========================= 
  UShort_t channelID;
  float energy;
  Long64_t time;
  float tqT;

  channelID=-9999;
  energy=-9999;
  time=-9999;
  tqT=-9999;

  //==============set Branch addresses for all the input variables================  
  anatree->SetBranchAddress("channelID",&channelID);
  anatree->SetBranchAddress("energy",&energy);
  anatree->SetBranchAddress("time",&time);
  anatree->SetBranchAddress("tqT",&tqT);

  //=====================Initialize output tree variables=========================
  vector<UShort_t> chID;
  Long64_t chTime[64];
  float chEnergy[64];
  float chTqT[64];
  Int_t event;
  event=1;

  //==============set Branch addresses for all the output variables================  
  outTree->Branch("event",&event,"event/I");
  outTree->Branch("chID","vector <UShort_t>",&chID);
  outTree->Branch("chTime",&chTime,"chTime[64]/L");
  outTree->Branch("chTqT",&chTqT,"chTqT[64]/F");
  outTree->Branch("chEnergy",&chEnergy,"chEnergy[64]/F");


 //========================Initialize local variables=================== 
  double co_window=500000.0; //(500 ns)    
  Int_t nentries_ana = (Int_t)anatree->GetEntries();

  Long64_t T_prev=300000000000;

  //======================== Look for coincidences =======================================

  for (Int_t i=0;i<nentries_ana; i++) {
    anatree->GetEntry(i);
    Long64_t t_diff=T_prev-time;
    //if(i<1000)cout<<t_diff<<endl;
    //if(i<1000)cout<<i<<" "<<channelID<<" "<<time<<endl;
    if(fabs(t_diff)>co_window && i!=0){
      outTree->Fill();
      chID.clear();
      for(int k=0;k<64;k++){
	chTime[k]=-9999;
	chEnergy[k]=-9999;
        chTqT[k]=-9999;
      }
      //if(i<1000)cout<<"===============================\n";
      event++;
    }

    T_prev=time;
    chID.push_back(channelID);
    chTime[channelID]=time;
    chEnergy[channelID]=energy;
    chTqT[channelID]=tqT;
    //if(i<1000)cout<<channelID<<" "<<chTime[channelID]<<" "<<chEnergy[channelID]<<endl;
  }

  AnaFile->Close();
  outFile->Write();
  outFile->Close();

}
