//
// Fit one event with a pulse shape
// To run:
// > root -l Example05.C+
//

#include "Pulse.h"
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TRandom.h>
#include <iostream>


Pulse pSh;


double funcPulseShape( double *x, double *par )
{
  double t = x[0] - par[0];
  return par[1] + par[2] * pSh.fShape(t);
}



void Example05()
{
  // Fit function
  
  TF1 *fPulseShape = new TF1("fPulseShape", funcPulseShape, -500, 500, 3);
  fPulseShape->SetNpx(1000);

  // Get one event with samples

  TFile *file2 = new TFile("data/samples_signal_10GeV_pu_0.root");
  //  TFile *file2 = new TFile("data/samples_signal_10GeV_eta_0.0_pu_140.root");

  double samples[NSAMPLES];
  double amplitudeTruth;
  TTree *tree = (TTree*)file2->Get("Samples");
  tree->SetBranchAddress("amplitudeTruth",      &amplitudeTruth);
  tree->SetBranchAddress("samples",             samples);

  tree->GetEntry(10);

  // Create TGraphErrors with the pulse to fit
  
  TGraphErrors *gr = new TGraphErrors();
  for(int i=0; i<10; i++){
    double x = i * NFREQ;  
    gr->SetPoint(i, x, samples[i]);
    gr->SetPointError(i, 0., 0.044);  // 44 MeV for all samples
  }

  // Fit 
  
  fPulseShape->SetParameters(70., 0., amplitudeTruth);
  fPulseShape->SetLineColor(2);
  gr->Fit("fPulseShape","E");
  gr->Draw("AP");
  

  // Errors in reconstructed timing and amplitude

  TH1D * h1 = new TH1D("h1", "amplitude", 1000, -5., 5.);
  TH1D * h2 = new TH1D("h2", "timing", 1000, 60., 80.);
  int nentries = tree->GetEntries();
  for(int ievt=0; ievt<nentries; ievt++){
    tree->GetEntry(ievt);
    TGraphErrors *gr = new TGraphErrors();
    for(int i=0; i<NSAMPLES; i++){
      double x = i * NFREQ;  
      gr->SetPoint(i, x, samples[i]);
      gr->SetPointError(i, 0., 0.044);  // 44 MeV for all samples
    }
    fPulseShape->SetParameters(70., 0., amplitudeTruth);
    gr->Fit("fPulseShape","QE");
    h1->Fill(fPulseShape->GetParameter(2)-amplitudeTruth);
    h2->Fill(fPulseShape->GetParameter(0));
  }
  cout << " amplitude uncertainty   = " << h1->GetRMS() << " GeV" << endl;
  cout << " timing uncertainty      = " << h2->GetRMS() << " ns" << endl;
  
  
}
