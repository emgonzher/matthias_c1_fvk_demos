#! /bin/bash


executable=matthias_disk_explorer_fvk
make $executable

main_dir=RESLT_eta/ea0001_radius_nodamping
if [ -e $main_dir ]; then
    echo " "
    echo "WARNING: Directory " $main_dir " already exists!"
    read -p "         remove it and continue? [Y/n] " yn
    case $yn in
        ''|[Yy]* ) rm -rf $main_dir;;
        [Nn]* ) echo "Can't continue until you move $main_dir"; exit;;
    esac
fi
mkdir $main_dir

#./$executable  > OUTPUT 

# Stuff to move
important_files="$executable  $executable.cc"

cp $important_files $main_dir
cd $main_dir

reslt_dir=RESLT
mkdir $reslt_dir

# Do it
#./$executable --test_damped_solve --use_polyline_for_internal_boundaries > OUTPUT 
# No damping test
echo " "
echo "LOOK! No damping..."
echo " "
./$executable --use_polyline_for_internal_boundaries > OUTPUT 
                            
# cd $main_dir
cd $reslt_dir
oomph-convert soln*.dat; makePvd soln soln.pvd 
oomph-convert full_soln*.dat; makePvd full_soln full_soln.pvd

echo " "
echo " "
echo "Done!"
echo " "
echo " "

exit
