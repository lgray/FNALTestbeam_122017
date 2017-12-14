#define data_cxx
#include "data.h"
#include "TH1F.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void data::Loop()
{
//   In a ROOT session, you can do:
//      root> .L data.C
//      root> data t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;
   TH1F* h = new TH1F("h", "h", 200000, -100000,100000);
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries-1;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (energy < 50) continue;
      // if (Cut(ientry) < 0) continue;
      //std::cout << time << std::endl;
      Long64_t time1 = time;
      UShort_t myID = channelID;
      nb = fChain->GetEntry(jentry+1);   nbytes += nb;
      time1 = time1-time;
      if( myID == channelID )continue;
      h->Fill(time1);
      std::cout << time1 << std::endl;
   }
   h->Draw();
}
