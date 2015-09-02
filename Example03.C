//
// Plots one waveform. To run it:
// > root -l Example03.C
//


#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TRandom.h>
#include <iostream>


void Example03()
{
  TFile *file = new TFile("data/waveform_signal_10GeV_eta_0.0_pu_140.root");

  int    nWF;
  double waveform[500];
  TTree *tree = (TTree*)file->Get("Waveforms");
  tree->SetBranchAddress("nWF",      &nWF);
  tree->SetBranchAddress("waveform", waveform);

  tree->GetEntry(10);
  
  TGraph *gr = new TGraph();
  for(int i=0; i<nWF; i++){
    gr->SetPoint(i, i, waveform[i]);
  }

  gr->Draw("AL");
}
