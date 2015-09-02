//
// Takes waveforms from the "file".
// Creates NSAMPLES starting with IDSTART time with the step of NFREQ ns
// Applies noise (correlated) for each sample
// Stores samples and true in-time amplitude
// To run:
// > root -l -q Example04.C+
//


#include "Pulse.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TProfile.h>
#include <TF1.h>
#include <TGraph.h>
#include <TRandom.h>
#include <TMath.h>
#include <iostream>



void Example04()
{
  Pulse pSh;
  
  TString filenameOutput = "output.root"; 
  
  // Noise level (GeV)
  double sigmaNoise = 0.044;
  

  // input Waveforms

  TFile *file = new TFile("data/waveform_signal_10GeV_pu_0.root");
  int    BX0;
  int    nWF;
  double waveform[WFLENGTH];
  double energyPU[NBXTOTAL];
  double signalTruth;
  TTree *tree = (TTree*)file->Get("Waveforms");
  tree->SetBranchAddress("nWF",          &nWF);
  tree->SetBranchAddress("waveform",     waveform);
  tree->SetBranchAddress("BX0",          &BX0);
  tree->SetBranchAddress("signalTruth",  &signalTruth);
  tree->SetBranchAddress("energyPU" ,    energyPU);

  // output samples
  
  int nSmpl = NSAMPLES;
  int nFreq = NFREQ;
  double samples[NSAMPLES];
  double amplitudeTruth;
  TFile *fileOut = new TFile(filenameOutput.Data(),"recreate");
  TTree *treeOut = new TTree("Samples", "");

  treeOut->Branch("nSmpl",             &nSmpl,               "nSmpl/I");
  treeOut->Branch("nFreq",             &nFreq,               "nFreq/I");
  treeOut->Branch("amplitudeTruth",    &amplitudeTruth,      "amplitudeTruth/D");
  treeOut->Branch("samples",           samples,              "samples[nSmpl]/D");
  
  int nentries = tree->GetEntries();
  for(int ievt=0; ievt<nentries; ievt++){

    double samplesUncorrelated[NSAMPLES];
    
    for(int i=0; i<NSAMPLES; ++i){
      samplesUncorrelated[i] = rnd.Gaus(0,1);
    }
    
    // Noise correlations
    for(int i=0; i<NSAMPLES; ++i){
      samples[i]=0;
      for(int j=0; j<NSAMPLES; ++j){
	samples[i] += pSh.cholesky(i,j) * samplesUncorrelated[j];
      }
    }

    for(int i=0; i<NSAMPLES; ++i){
      samples[i]   *= sigmaNoise;
    }
 
    // add signal and pileup
    
    tree->GetEntry(ievt);
    for(int i=0; i<NSAMPLES; ++i){
      int index = IDSTART + i * NFREQ;
      samples[i]   += waveform[index];
    }    

    // true amplitude = in-time pileup + signal
    amplitudeTruth = signalTruth + energyPU[BX0];

    treeOut->Fill();
  }
  
  treeOut->Write();
  fileOut->Close();
  file->Close();

}
