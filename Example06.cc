//
// MultiFit amplitude reconstruction
// To run:
// > g++ -o Example06 Example06.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
// > ./Example06
//

#include <iostream>
#include "PulseChiSqSNNLS.h"
#include "Pulse.h"

#include "TTree.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2.h"
#include "TFile.h"
 
using namespace std;

Pulse pSh;

FullSampleVector fullpulse(FullSampleVector::Zero());
FullSampleMatrix fullpulsecov(FullSampleMatrix::Zero());
SampleMatrix noisecor(SampleMatrix::Zero());
BXVector activeBX;
SampleVector amplitudes(SampleVector::Zero());

TFile *fout;
TH1D *h01;



void initHist()
{
  fout = new TFile("output.root","recreate");
  h01 = new TH1D("h01", "dA", 1000, -5.0, 5.0);
}

void init()
{
  initHist();

  // intime sample is [2]
  double pulseShapeTemplate[NSAMPLES+2];
  for(int i=0; i<(NSAMPLES+2); i++){
    double x = double( IDSTART + NFREQ * (i + 3) - WFLENGTH / 2);
    pulseShapeTemplate[i] = pSh.fShape(x);
  }
  //  for(int i=0; i<(NSAMPLES+2); i++) pulseShapeTemplate[i] /= pulseShapeTemplate[2];
  for (int i=0; i<(NSAMPLES+2); ++i) fullpulse(i+7) = pulseShapeTemplate[i];

  
  for (int i=0; i<NSAMPLES; ++i) {
    for (int j=0; j<NSAMPLES; ++j) {
      int vidx = std::abs(j-i);
      noisecor(i,j) = pSh.corr(vidx);
    }
  }

  int activeBXs[] = { -5, -4, -3, -2, -1,  0,  1,  2,  3,  4 };
  activeBX.resize(10);
  for (unsigned int ibx=0; ibx<10; ++ibx) {
    activeBX.coeffRef(ibx) = activeBXs[ibx];
  } 
  //  activeBX.resize(1);
  //  activeBX.coeffRef(0) = 0;
}



void run()
{

  TFile *file2 = new TFile("data/samples_signal_10GeV_pu_0.root");
  //  TFile *file2 = new TFile("data/samples_signal_10GeV_eta_0.0_pu_140.root");

  double samples[NSAMPLES];
  double amplitudeTruth;
  TTree *tree = (TTree*)file2->Get("Samples");
  tree->SetBranchAddress("amplitudeTruth",      &amplitudeTruth);
  tree->SetBranchAddress("samples",             samples);
  int nentries = tree->GetEntries();

  for(int ievt=0; ievt<nentries; ++ievt){
    tree->GetEntry(ievt);
    for(int i=0; i<NSAMPLES; i++){
      amplitudes[i] = samples[i];
    }

    double pedval = 0.;
    double pedrms = 1.0;
    PulseChiSqSNNLS pulsefunc;

    pulsefunc.disableErrorCalculation();
    bool status = pulsefunc.DoFit(amplitudes,noisecor,pedrms,activeBX,fullpulse,fullpulsecov);
    double chisq = pulsefunc.ChiSq();
  
    unsigned int ipulseintime = 0;
    for (unsigned int ipulse=0; ipulse<pulsefunc.BXs().rows(); ++ipulse) {
      if (pulsefunc.BXs().coeff(ipulse)==0) {
	ipulseintime = ipulse;
	break;
      }
    }
    double aMax = status ? pulsefunc.X()[ipulseintime] : 0.;
    //  double aErr = status ? pulsefunc.Errors()[ipulseintime] : 0.;

    h01->Fill(aMax - amplitudeTruth);
  }
  cout << "  Mean of REC-MC = " << h01->GetMean() << " GeV" << endl;
  cout << "   RMS of REC-MC = " << h01->GetRMS() << " GeV" << endl;
}

void saveHist()
{

  fout->cd();
  h01->Write();
  fout->Close();
}



# ifndef __CINT__
int main()
{
  init();
  run();
  saveHist();
  return 0;
}
# endif
