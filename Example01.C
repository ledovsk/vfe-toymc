//
// Initialize Event file
// Store Pulse shape as a TGraph and three parameters to describe the tail of the pulse shape
// Original graph is stored in data/crrc43ns.root
//
// Suggestion to run it:
// >root -l -q Example01.C
// >cp output.root filename.root
//
#include <TFile.h>
#include <TGraph.h>
#include <TTree.h>
#include <TH1.h>
#include <iostream>


void Example01()
{
  TFile *fin = new TFile("data/crrc43ns.root");
  TGraph *grPulseShape = (TGraph*)fin->Get("grCRRC43");
  grPulseShape->SetName("grPulseShape");
  grPulseShape->SetTitle("Pulse shape");

  TFile *fout = new TFile("output.root","recreate");
  TDirectory *cdPulseShape = fout->mkdir("PulseShape");
  cdPulseShape->cd();
  grPulseShape->Write();

  TTree *tr = new TTree("Tail","Parameters for the tail of the pulse shape");
  float timeMin;
  float expAmplitude;
  float expTime;
  tr->Branch("timeMin",     &timeMin,     "timeMin/F");
  tr->Branch("expAmplitude",&expAmplitude,"expAmplitude/F");
  tr->Branch("expTime",     &expTime,     "expTime/F");
  timeMin      = 800.;
  expAmplitude = 0.0401833;
  expTime      = 1. / 100.;
  tr->Fill();
  tr->Write();
  fout->cd();
  
  TFile *fin2 = new TFile("data/minbias_01.root");
  TDirectory *cdPU = fout->mkdir("PileupPDFs");
  cdPU->cd();
  TH1D *hpdf[14];
  for(int ih=0; ih<14; ih++){
    char hnameInput[120];
    sprintf(hnameInput,"h%i",101+ih);
    char hnameOutput[12];
    sprintf(hnameOutput,"pupdf_%i",ih);
    char htitleOutput[12];
    sprintf(htitleOutput,"pdf of energies from minbias interaction at %3.2f < eta < %3.2f", 0.05+0.1*ih, 0.15+0.1*ih);
    hpdf[ih] = (TH1D*)fin2->Get(hnameInput);
    //    hpdf[ih]->SetDirectory(cdPU);
    hpdf[ih]->SetName(hnameOutput);
    hpdf[ih]->SetTitle(htitleOutput);
  }
  for(int ih=0; ih<14; ih++){
    cdPU->cd();
    hpdf[ih]->Write();
  }
  fout->cd();
  fout->Close();

}
