#! /bin/bash

ccfilename=matthias_disk_explorer
executable=matthias_disk_explorer_fvk

#!/bin/bash

#!/bin/bash

# default: no auto-remove
AUTO_REMOVE=0

# parse command-line arguments
for arg in "$@"; do
    case $arg in
        --autoremove)
            AUTO_REMOVE=1
            shift
            ;;
        *)
            echo "Usage: $0 [--autoremove]"
            exit 1
            ;;
    esac
done

main_dir=RESLT_eta/ea0001_radius

if [ -e "$main_dir" ]; then
    if [ "$AUTO_REMOVE" -eq 1 ]; then
        echo "Removing $main_dir automatically..."
        rm -rf "$main_dir"
    else
        if [ -t 0 ]; then
            # interactive shell → ask user
            echo " "
            echo "WARNING: Directory $main_dir already exists!"
            read -p "         remove it and continue? [Y/n] " yn
            case $yn in
                ''|[Yy]* ) rm -rf "$main_dir";;
                [Nn]* ) echo "Can't continue until you move $main_dir"; exit;;
            esac
        else
            # non-interactive shell without --autoremove → fail safely
            echo "Error: $main_dir exists, and cannot prompt in non-interactive mode."
            exit 1
        fi
    fi
fi

mkdir "$main_dir"

make $executable

#./$executable  > OUTPUT 

# Stuff to move
important_files="$executable  $ccfilename.cc"


cp $important_files $main_dir
cd $main_dir

reslt_dir=RESLT
mkdir $reslt_dir
reslt_damped_dir=RESLT_DAMPED
mkdir $reslt_damped_dir

# Do it
./$executable --test_damped_solve --use_polyline_for_internal_boundaries > OUTPUT 

# No damping test
#echo " "
#echo "LOOK! No damping..."
#echo " "
#./$executable --use_polyline_for_internal_boundaries > OUTPUT 
                            
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
