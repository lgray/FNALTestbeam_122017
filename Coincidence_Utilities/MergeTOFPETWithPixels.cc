#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include <assert.h>

using namespace std;

struct FTBFPixelEvent {
    double xSlope;
    double ySlope;
    double xIntercept;
    double yIntercept;
    double chi2;
    int trigger;
    int runNumber;
    Long64_t timestamp;    
    Long64_t bco;    
};


std::string ParseCommandLine( int argc, char* argv[], std::string opt )
{
  for (int i = 1; i < argc; i++ )
    {
      std::string tmp( argv[i] );
      if ( tmp.find( opt ) != std::string::npos )
        {
          if ( tmp.find( "=" )  != std::string::npos ) return tmp.substr( tmp.find_last_of("=") + 1 );
	  if ( tmp.find( "--" ) != std::string::npos ) return "yes";
	}
    }
  
  return "";
};



//get list of files to open, add normalization branch to the tree in each file
int main(int argc, char* argv[]) {

  //parse input list to get names of ROOT files
  if(argc < 4){
    cerr << "usage MergeTOFPETWithDRS <PixelFile> <NimPlusFile> <TOFPETEventFile> <OutputFile>" << endl;
    return -1;
  }
  int debugLevel = -1;
  const char *PixelInputFileName = argv[1];
  const char *NimPlusInputFileName = argv[2];
  const char *TOFPETEventFileName   = argv[3];
  const char *outFileName   = argv[4];

  //**********************************
  //First read in the NimPlus timestamps
  //**********************************
  long triggerNumber=-1;
  vector<long long> NimPlusTimestamp;
  FILE* NimPlusInputFile = fopen( NimPlusInputFileName, "rb" );
  //cout << NimPlusInputFileName << endl;
  //int cnt = 0;
  //while(!feof(NimPlusInputFile) && cnt++ < 5)
  //{

  //  cout << fgetc(NimPlusInputFile) << endl;
  // }
  //return 0;
  //ifstream myfile;
  //myfile.open(NimPlusInputFileName, ios::in | ios::binary);
  long long Prev_tS = -9999; //first time stamp is set to 0
  long long curr_tS = -9999;
  int eventWordIndex=0;
  bool isNewTrigger = false;
  int maxPackets = 999999999;
  for( int iPacket = 0; iPacket < maxPackets; iPacket++){ 
  //while(!feof(NimPlusInputFile)){
    long tmpTrigger = 0;
    long long tmpTimestamp = 0;
    long long tmpWord = 0;
    
    long tmp = 0;
    long QuadNo=0;
    long long t_diff=0;
    //cout << "Event: " << iPacket << " : ";    

    int x;
    // int k=-4;
    //if(iPacket<5){myfile>>x;cout<<static_cast<long>(x)<<endl;}
    //if(iPacket<5){myfile>>x;cout<<static_cast<long>(x)<<endl;}
    //unsigned char tmpC; 
    fread( &QuadNo, 1, 1, NimPlusInputFile); //no. of quad words in each packet (1 quadword= 8 bytes= 64 bits)
    if (debugLevel > 100) cout <<QuadNo << " ";
    fread( &tmp, 1, 1, NimPlusInputFile); //packet type -- 1,2 or 3
    if (debugLevel > 100) cout << tmp << " ";
    fread( &tmp, 1, 1, NimPlusInputFile); // sequence ID -- increments by 1 each time
    if (debugLevel > 100) cout << tmp << "\n ";
    for(int i=0;i<QuadNo*2;i++){

      //read 32-bit words
      fread( &tmpWord, sizeof(float), 1, NimPlusInputFile); 
      if (debugLevel > 100) cout << tmpWord << "(k=" << eventWordIndex << ") ";

      //this is the trigger number word
      if (eventWordIndex%6==2) {
	//a new trigger
	if (tmpWord > triggerNumber) {
	  triggerNumber++;
	  isNewTrigger = true;
	  if (debugLevel > 10) cout << "Trigger Number: " << tmpWord << " : ";
	}
      }

      if(eventWordIndex%6==4){
	if (isNewTrigger) {
	  NimPlusTimestamp.push_back(tmpWord);
	  if (debugLevel > 10) cout << tmpWord << "\n";
	  isNewTrigger = false;
	}

      	// if(k==0 && iPacket==0){
      	//   Prev_tS=tmpTimestamp;//set first timeStamp at start of run to 0
      	//   //tmpTimestamp=0.;
      	// }
      	// curr_tS=tmpTimestamp;
      	// t_diff=(curr_tS-Prev_tS)*3; // time difference in ns
      	// if(t_diff*0.000000001>60.)cout<<"\n xxxxxxxxxxxxxxxxxxxxxxx"<<iPacket<<" "<<t_diff<<endl; // Change of Spill
      	// Prev_tS=curr_tS;
      }
      eventWordIndex++;

    }
    if (debugLevel > 100) cout << "\n--------------------------------------------------------------------------------\n";
    // // check for end of file
    if (feof(NimPlusInputFile)) break;
  }

  for (int i=0; i<NimPlusTimestamp.size();i++) {
    cout << "Trigger: " << i << " " << NimPlusTimestamp[i] << "\n";
  }


  return 0;





  //create output file
  TFile *outputFile = new TFile(outFileName, "RECREATE");

  //loop over all TTrees in the file and add the weight branch to each of them
  TFile *PixelInputFile = TFile::Open(PixelInputFileName, "READ");
  TFile *TOFPETEventFile = TFile::Open(TOFPETEventFileName, "READ");
  assert(PixelInputFile);
  PixelInputFile->cd();
  assert(TOFPETEventFile);

  TTree *PixelTree = (TTree*)PixelInputFile->Get("MAPSA");
  TTree *TOFPETEventTree = (TTree*)TOFPETEventFile->Get("data");
 
  //create new normalized tree
  outputFile->cd();
  TTree *outputTree = PixelTree->CloneTree(0);
  //cout << "Events in the ntuple: " << PixelTree->GetEntries() << endl;
  
  //branches for the TOFPET tree
  std::vector<UShort_t> *chID = 0;               
  UShort_t outputCHID[64];
  Long64_t chTime[64];
  float chEnergy[64];
  float chTqT[64];
  Int_t event;
  // outputTree->Branch("chID","vector <UShort_t>",&chID);
  outputTree->Branch("chID",&outputCHID,"chID[64]/s");
  outputTree->Branch("chTime",&chTime,"chTime[64]/L");
  outputTree->Branch("chTqT",&chTqT,"chTqT[64]/F");
  outputTree->Branch("chEnergy",&chEnergy,"chEnergy[64]/F");
  TOFPETEventTree->SetBranchAddress("event",&event);
  TOFPETEventTree->SetBranchAddress("chID",&chID);
  TOFPETEventTree->SetBranchAddress("chTime",&chTime);
  TOFPETEventTree->SetBranchAddress("chTqT",&chTqT);
  TOFPETEventTree->SetBranchAddress("chEnergy",&chEnergy);

  //branches for the PixelTree
  FTBFPixelEvent pixelEvent;
  PixelTree->SetBranchAddress("event",&pixelEvent);




  int TOFPETEventIndex = 0;
  Long64_t FirstEventTimeTOFPET = -999;
  Long64_t FirstEventTimePixel = -999;

  Long64_t PreviousEventTimePixel = -999;
  int PreviousEventTimeTrigger = 0;
  Long64_t PreviousEventTimeTOFPET = -999;
  int TOFPETTriggerCounter = 1;


  // for (int n=0;n<PixelTree->GetEntries();n++) { 
  for (int n=0;n<100;n++) { 
    //if (n%100==0) cout << "Processed Event " << n << "\n";
    PixelTree->GetEntry(n);
    
    if (n==0) FirstEventTimePixel = pixelEvent.bco; 
    
    if (!(pixelEvent.bco == -1 || pixelEvent.bco == 923)) {
      //cout << "Event: " << n << " | " << pixelEvent.trigger << " " << pixelEvent.bco*74e-9 << " | " << (pixelEvent.bco - PreviousEventTimePixel)*74 << "\n";
      PreviousEventTimePixel = pixelEvent.bco;
      PreviousEventTimeTrigger = pixelEvent.trigger;
    }
  }
  return 0;


  //    for(int q=TOFPETEventIndex; q < TOFPETEventTree->GetEntries(); q++) {
  for(int q=0; q < TOFPETEventTree->GetEntries(); q++) {
    TOFPETEventTree->GetEntry(q);
    
    if (q==0) FirstEventTimeTOFPET = chTime[41];
    
    if (chEnergy[32] > -9000 && chEnergy[41] > -9000) {
      
      //cout << "Event: " << TOFPETTriggerCounter << " | " << pixelEvent.trigger << " " << pixelEvent.bco << " | " << pixelEvent.bco - FirstEventTimePixel <<  " " << 1e-9*18.6*4*(pixelEvent.bco - FirstEventTimePixel)/pixelEvent.trigger << " | " << pixelEvent.bco - PreviousEventTimePixel << " " << 1e-9*18.6*4*(pixelEvent.bco - PreviousEventTimePixel) /(pixelEvent.trigger-PreviousEventTimeTrigger) << "\n";
      TOFPETTriggerCounter++;
    }
     
    // //match
    // if (false) {
    //   TOFPETEventIndex = 0;
    //   foundMatch = true;
    //   break;
    // }
  }

  
  return 0;



    // //********************************************
    // //Find matching event in TOFPETEventTree;
    // //********************************************
    // bool foundMatch = false;
    // // //    for(int q=TOFPETEventIndex; q < TOFPETEventTree->GetEntries(); q++) {
    // // for(int q=0; q < TOFPETEventTree->GetEntries(); q++) {
    // //   TOFPETEventTree->GetEntry(q);
      
    // //   if (q==0) FirstEventTimeTracker = 

    // //   //match
    // //   if (false) {
    // // 	TOFPETEventIndex = 0;
    // // 	foundMatch = true;
    // // 	break;
    // //   }
    // // }

    // //Successfully Matched, so increment TOFPET Event Index
    // if (foundMatch) {
    //   cout << "matched event : " << TOFPETEventIndex << " | " 
    // 	   << chEnergy[41] << " " << chTime[41] 
    // 	   << "\n";

    //   TOFPETEventIndex++;
    // } else {
    //   for (int k=0;k<64;k++) {
    // 	outputCHID[k] = -1;
    // 	chTime[k] = -999;
    // 	chTqT[k] = -999;
    // 	chEnergy[k] = -999;
    //   }
    // }
    
    // outputTree->Fill();
   



    


  //save
  outputTree->Write();

  //Close input files
  PixelInputFile->Close();
  TOFPETEventFile->Close();
  //cout << "Closing output file." << endl;
  outputFile->Close();
  delete outputFile;
}
