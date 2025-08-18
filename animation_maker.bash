#! /bin/bash
# This script creates the pvd files needed for compare_animation.pvsm

# Change directory to PostProcess data (PPDATA)
cd ./PPDAT

# 1) convert files to vtu
 oomph-convert full*.dat averaged*.dat pert*.dat
# 2) create pvd files
 makePvd averaged axiaver.pvd; makePvd pert pert.pvd; makePvd full fulline.pvd

# Retunr to RESLT
 cd ..
 

