#include <TRandom.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>

TRandom rnd;

// total number of bunches in "LHC" bunch train
const int NBXTOTAL = 2800;

// length of a waveform in 1ns steps
const int WFLENGTH  = 500;

// number of samples per hit
const int NSAMPLES   = 10;

// distance between samples in 1ns steps
const int NFREQ      = 25;

// position of a 1st sample inside waveform
const int IDSTART    = 180;

// CRRC shaping time in ns. It is used to calculate noise
// correlations. For QIE, set it to 1e-1
const double TAU = 43.0;

// filename with pulse shape
const TString FNAMESHAPE       = "data/EmptyFileCRRC43.root";



class Pulse{

  double weights_[NSAMPLES];
  double mC_[NSAMPLES];
  double mL_[NSAMPLES][NSAMPLES];
  TGraph *grPS_;
  float tMin_;
  float fPar0_;
  float fPar1_;
  TFile *filePS_;

 public:

  Pulse();
  ~Pulse();
  TGraph* grPS() {return grPS_; };
  float tMin() const { return tMin_; };
  float fPar0() const { return fPar0_; };
  float fPar1() const { return fPar1_; };
  double weight(int i) const { return weights_[i]; };
  double corr(int i) const { return mC_[i]; };
  double cholesky(int i, int j) const { return mL_[i][j]; };
  void NoiseInit();
  double fShape(double);
  
};


Pulse::Pulse()
{
  filePS_ = new TFile(FNAMESHAPE.Data());
  grPS_ = (TGraph*)filePS_->Get("PulseShape/grPulseShape");
  TTree *trPS = (TTree*)filePS_->Get("PulseShape/Tail");
  trPS->SetBranchAddress("timeMin",      &tMin_);
  trPS->SetBranchAddress("expAmplitude", &fPar0_);
  trPS->SetBranchAddress("expTime",      &fPar1_);
  trPS->GetEntry(0);

  // In-time sample is i=5
  
  for(int i=0; i<NSAMPLES; i++){
    double x = double( IDSTART + NFREQ * i - WFLENGTH / 2);
    weights_[i] = fShape(x);
  }

  NoiseInit();
}


Pulse::~Pulse()
{
}


double Pulse::fShape(double x)
{
  if( grPS_ !=0 && x > 0.){
    if(x<800.){
      return grPS_->Eval(x);
    }else{
      return fPar0_ * exp( -x * fPar1_ );
    }
  }else{
    return 0.;
  }
}



void Pulse::NoiseInit()
{
  for(int i=0; i<NSAMPLES; i++){
    double y = 1. - exp( -double(NFREQ * i) / (sqrt(2.) * TAU));
    mC_[i] = 1. - y * y;
  }

  // initialize
  for(int i=0; i<NSAMPLES; ++i){
    for(int j=0; j<NSAMPLES; ++j){
      mL_[i][j]=0;
    }
  }

  // decomposition
  mL_[0][0] = sqrt(mC_[0]);
  for( int col=1; col<NSAMPLES; col++){
    mL_[0][col]=0;
  }
  for( int row=1; row<NSAMPLES; row++){
    for( int col=0; col<row; col++ ){
      double sum1 = 0;
      int m=abs(row-col);
      for( int k=0; k<col; ++k) sum1 += mL_[row][k]*mL_[col][k];
      mL_[row][col] = (mC_[m] - sum1)/mL_[col][col];
    }
    double sum2 = 0;
    for( int k=0; k<row; ++k) sum2 += mL_[row][k]*mL_[row][k];
    mL_[row][row] = sqrt( mC_[0] - sum2 );
    for( int col=row+1; col<NSAMPLES; col++ ) mL_[row][col] = 0;
  }
}



