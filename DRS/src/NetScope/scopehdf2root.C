#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <math.h>

// C++ includes
#include <fstream>
#include <string>
#include <iostream>

// ROOT includes
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

#include "common.h"
#include "hdf5io.h"

char *waveformBuf;

using namespace std;

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

int main(int argc, char **argv) {
  size_t i, j, iCh, iEvent=0, nEvents=0, frameSize, nEventsInFile;
  char *inFileName;
  
  struct hdf5io_waveform_file *waveformFile;
  struct waveform_attribute waveformAttr;
  struct hdf5io_waveform_event waveformEvent;
  
  if(argc<2) {
    std::cout << "Usage:" << std::endl;
    std::cout << "   " << argv[0] << " -ifile=input.hdf5 -ofile=output.root" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "   -iEvent=#event (starting event number)" << std::endl;
    std::cout << "   -nEvents=#events (total number of events to run over)" << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFilename = ParseCommandLine( argc, argv, "ifile" );
  std::cout << "Opening input file " << inputFilename << std::endl;
  waveformFile = hdf5io_open_file_for_read(inputFilename.c_str());

  std::string s_iEvent = ParseCommandLine( argc, argv, "iEvent" );
  if(s_iEvent != "")
    iEvent = atol(s_iEvent.c_str());
  
  std::string s_nEvents = ParseCommandLine( argc, argv, "nEvents" );
  if(s_nEvents != "")
    nEvents = atol(s_nEvents.c_str());
  
  hdf5io_read_waveform_attribute_in_file_header(waveformFile, &waveformAttr);
  fprintf(stderr, "waveform_attribute:\n"
	  "     chMask  = 0x%02x\n"
	  "     nPt     = %zd\n"
	  "     nFrames = %zd\n"
	  "     dt      = %g\n"
	  "     t0      = %g\n"
	  "     ymult   = %g %g %g %g\n"
	  "     yoff    = %g %g %g %g\n"
	  "     yzero   = %g %g %g %g\n",
	  waveformAttr.chMask, waveformAttr.nPt, waveformAttr.nFrames, waveformAttr.dt,
	  waveformAttr.t0, waveformAttr.ymult[0], waveformAttr.ymult[1], waveformAttr.ymult[2],
	  waveformAttr.ymult[3], waveformAttr.yoff[0], waveformAttr.yoff[1],
	  waveformAttr.yoff[2], waveformAttr.yoff[3], waveformAttr.yzero[0],
	  waveformAttr.yzero[1], waveformAttr.yzero[2], waveformAttr.yzero[3]);
  
  nEventsInFile = hdf5io_get_number_of_events(waveformFile);
  fprintf(stderr, "Number of events in file: %zd\n", nEventsInFile);
  if(nEvents <= 0 || nEvents+iEvent >= nEventsInFile) nEvents = nEventsInFile-iEvent;
  if(waveformAttr.nFrames > 0) {
    frameSize = waveformAttr.nPt / waveformAttr.nFrames;
    fprintf(stderr, "Frame size: %zd\n", frameSize);
  } else {
    frameSize = waveformAttr.nPt;
  }

  waveformBuf = (char*)malloc(waveformFile->nPt * waveformFile->nCh * sizeof(char));
  waveformEvent.wavBuf = waveformBuf;
  
  std::string outputFilename = ParseCommandLine( argc, argv, "ofile" );
  std::cout << "Initializing output file " << outputFilename << std::endl;
  TFile* ofile = new TFile( outputFilename.c_str(), "RECREATE", "NetScope");
  TTree* tree = new TTree("pulse", "Digitized waveforms");
  
  // initialization of variables for TTree output
  int event;
  float time[2][1000]; // calibrated time
  float channel[4][1000]; // input (in V)
  float xmin[4]; // location of peak
  float base[4]; // baseline voltage
  float amp[4]; // pulse amplitude
  float integral[4]; // integral in a window
  float integralFull[4]; // integral over all bins
  float gauspeak[4]; // time extracted with gaussian fit
  float sigmoidTime[4];//time extracted with sigmoid fit
  float fullFitTime[4];//time extracted with sigmoid fit
  float linearTime0[4]; // constant fraction fit coordinates
  float linearTime15[4];
  float linearTime30[4];
  float linearTime45[4];
  float linearTime60[4];
  float fallingTime[4]; // falling exponential timestamp    
  float risetime[4]; 
  float constantThresholdTime[4];
  bool _isRinging[4];

  tree->Branch("event", &event, "event/I");
  tree->Branch("channel", channel, "channel[4][1000]/F");
  tree->Branch("time", time, "time[2][1000]/F");
  tree->Branch("xmin", xmin, "xmin[4]/F");
  tree->Branch("amp", amp, "amp[4]/F");
  tree->Branch("base", base, "base[4]/F");
  tree->Branch("integral", integral, "integral[4]/F");
  tree->Branch("intfull", integralFull, "intfull[4]/F");
  tree->Branch("gauspeak", gauspeak, "gauspeak[4]/F");
  tree->Branch("sigmoidTime", sigmoidTime, "sigmoidTime[4]/F");
  tree->Branch("fullFitTime", fullFitTime, "fullFitTime[4]/F");
  tree->Branch("linearTime0", linearTime0, "linearTime0[4]/F");
  tree->Branch("linearTime15", linearTime15, "linearTime15[4]/F");
  tree->Branch("linearTime30", linearTime30, "linearTime30[4]/F");
  tree->Branch("linearTime45", linearTime45, "linearTime45[4]/F");
  tree->Branch("linearTime60", linearTime60, "linearTime60[4]/F");
  tree->Branch("fallingTime", fallingTime, "fallingTime[4]/F");
  tree->Branch("risetime", risetime, "risetime[4]/F");
  tree->Branch("constantThresholdTime", constantThresholdTime, "constantThresholdTime[4]/F");
  tree->Branch("isRinging", _isRinging, "isRinging[4]/O");
  
  for(int i = 0; i < 1000; i++){
    time[0][i] = 0.;
    time[1][i] = 0.;
    for(int iCh = 0; iCh < 4; iCh++){
      channel[iCh][i] = 0.;
    }
  }
  
  // Event loop
  for(waveformEvent.eventId = iEvent; waveformEvent.eventId < iEvent + nEvents;
      waveformEvent.eventId++) {
    hdf5io_read_event(waveformFile, &waveformEvent);
    
    event = waveformEvent.eventId;
    
    for(i = 0; i < 1000; i++) {
      if(i < waveformFile->nPt)
	time[0][i] = waveformAttr.dt*(i%frameSize);
      else
	time[0][i] = 0.;
      //printf("%24.16e ", waveformAttr.dt*(i%frameSize));
      j = 0;
      for(iCh=0; iCh<SCOPE_NCH; iCh++) {
	if((1<<iCh) & waveformAttr.chMask) {
	  if(i < waveformFile->nPt){
	    channel[iCh][i] = (waveformBuf[j * waveformFile->nPt + i]
			       - waveformAttr.yoff[iCh])
	      * waveformAttr.ymult[iCh]
	      + waveformAttr.yzero[iCh];
	  } else {
	    channel[iCh][i] = 0.;
	  }
	  // printf("%24.16e ", (waveformBuf[j * waveformFile->nPt + i]
	  // 		      - waveformAttr.yoff[iCh])
	  // 	 * waveformAttr.ymult[iCh]
	  // 	 + waveformAttr.yzero[iCh]);
	  j++;
	}
      }
      // printf("\n");
      // if((i+1) % frameSize == 0)
      // 	printf("\n");
    }
    // printf("\n");
    tree->Fill();
  }

  ofile->cd();
  tree->Write();
  ofile->Close();
  
  free(waveformBuf);
  hdf5io_close_file(waveformFile);
    
  return EXIT_SUCCESS;
}
