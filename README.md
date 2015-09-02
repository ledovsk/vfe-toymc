# vfe-toymc
Toy MC to study VFE pulse shapes.


A set of examples for standalone.
It is a toy Monte Carlo for simulation and reconstruction of pulses.
All files here are for VFE scenario: CRRC 43ns pulse shaping
It is defined in Pulse.h

Example01
---------

Produces a file with information about pulse shape.  The output is a
root file with TGraph (pulse shape) and 3 parameters to describe the
tail of the pulse shape.

File data/EmptyFileCRRC43.root was produced with this example. It
corresponds to CRRC pulse shaper with tau=43ns. This file is a
starting point. This example is for info only. It does not need to be
repeated for CRRC 43ns shaper.


Example02
---------

Produces waveforms and stores it in a separate file. Each waveform is
an array of amplitudes (GeV) as a function of time (ns) in 500 ns time
window. Pileup level and signal amplitude need to be defined in the
code.

Files

data/waveform_signal_10GeV_pu_0.root 
data/waveform_signal_10GeV_eta_0.0_pu_140.root

were produced with this example. It is a set of 100 waveforms for
a signal of 10 GeV and PU=0 and PU=140 respectively.


Example03
---------

Plots one waveform created in Example02


Example04
---------

Creates DIGIs: samples of amplitude with noise.
Noise is correlated.

Files

data/samples_signal_10GeV_pu_0.root 
data/samples_signal_10GeV_eta_0.0_pu_140.root

were created from

data/waveforms_signal_10GeV_pu_0.root 
data/waveforms_signal_10GeV_eta_0.0_pu_140.root

with average noise of 44 MeV and correlation matrix for CRRC 43ns



Example05
---------

Fit one event with a pulse shape and plot it
Compare reconstructed amplitude with MC truth.
Timing is also reconstructed.
Errors on amplitude and timing are estimated.


Example06
---------

Multifit reconstruction
One need to make sure that Eigen is installed
To compile:
> g++ -o Example06 Example06.cc PulseChiSqSNNLS.cc -std=c++11 `root-config --cflags --glibs`
To run
> ./Example06 
