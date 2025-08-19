#! /bin/bash


# Setup directories YOU MUST PICK A NAME FOR YOUR OURPUT DIRECTORY.
<<<<<<< Updated upstream
main_dir=RESLT_damped/ea005_pinc1_damped #Run_ea05_pcos01_pinc02_n10_test #nseg4_Eta_141e5_pinc1_poly_pitchfork_n6_2lines #pcos1
=======
main_dir=RESLT_pitchfork/ea0001_pcos01_pinc01_N6 #nseg4_Eta_141e5_pinc1_poly_pitchfork_n6_2lines #pcos1
>>>>>>> Stashed changes
# main_dir=Run_pitchfork_ea005_nseg4_Eta_141e5_pcos01_pinc01
if [ -e $main_dir ]; then
    echo " "
    echo "WARNING: Directory " $main_dir " already exists!"
    read -p "         remove it and continue? [Y/n] " yn
    case $yn in
        ''|[Yy]* ) rm -rf $main_dir;;
        [Nn]* ) echo "Can't continue until you move $main_dir"; exit;;
    esac
fi
# == Remove without asking -- Just to run in background
# if [ -e $main_dir ]; then
#   rm -rf $main_dir
# fi
# =====
mkdir $main_dir


stem=test_disk_damped #test_disk_restart  
postfix_list="_fvk" #  _ks"
prev_sol_arg="" #"--use_prev_sol"
restart_file="" #"restart9.dat" #
for postfix in `echo $postfix_list`; do
    
    executable=$stem$postfix

    echo "Executable : "$executable
    make $executable

    # Stuff to move
    important_files="$executable  $stem.cc"
    
    case_list="_free" #--mod_2 #_balance_on_edge " # " _free " # _clamped _pinned  _free _balance_on_edge"
    echo $case_list
    for the_case in `echo $case_list`; do

        inner_boundary_list="straight" # " curved"
        for inner_boundary in `echo $inner_boundary_list`; do

            boundary_postfix="_straight"
            boundary_command_line_arg=" --use_polyline_for_internal_boundaries " # check
            if [ "$inner_boundary" == "curved" ]; then
                boundary_postfix="_curved"
                boundary_command_line_arg=" "
            fi
            
            t_shape_list="not_t_shaped" # "t_shaped not_t_shaped"
            for t_shape in `echo $t_shape_list`; do
                
                t_shape_postfix="_use_t_shape"
                t_shape_command_line_arg=" --use_t_shape_internal_boundaries "
                if [ "$t_shape" == "not_t_shaped" ]; then
                    t_shape_postfix="_no_t_shape"
                    t_shape_command_line_arg=" "
                fi              
                
                rotate_list="rotate" #" not_rotate" #miraqui--mod_1
                #rotate_list="not_rotate" 
                for rotate in `echo $rotate_list`; do
                    
                    rotate_postfix="_rotated_coords"
                    rotate_command_line_arg=" "
                    if [ "$rotate" == "not_rotate" ]; then
                        rotate_postfix="_unrotated_coords"
                        rotate_command_line_arg=" --do_not_rotate_coords_on_curved_boundaries "
                    fi              
                    
                    the_dir=$main_dir/Case$boundary_postfix$rotate_postfix$t_shape_postfix$postfix$the_case
                    
                    mkdir $the_dir
                    home_dir=`pwd`
                    
                    # Transfer the important files to the main directory and go there
                    cp $important_files $restart_file $the_dir #miraqui restart_file - why copied here?
                    cd $the_dir
                    
                    reslt_dir=RESLT
                    mkdir $reslt_dir
                    
                    args="--nplot 5 "$boundary_command_line_arg" "$t_shape_command_line_arg" "$rotate_command_line_arg
                    
                    if [ "$prev_sol_arg" == "--use_prev_sol" ]; then
                    #args="--"$restart_file" --nplot 5 "$boundary_command_line_arg" "$t_shape_command_line_arg" "$rotate_command_line_arg" "$prev_sol_arg
                    args="--nplot 5 "$boundary_command_line_arg" "$t_shape_command_line_arg" "$rotate_command_line_arg" "$prev_sol_arg
                    echo "using previous solution to restart"
                    fi


                    if [ "$the_case" == "_clamped" ]; then
                        args=$args" --use_clamped_bc "
                    elif [ "$the_case" == "_pinned" ]; then
                        args=$args" --use_pinned_bc "
                    elif [ "$the_case" == "_balance_on_edge" ]; then
                        args=$args" --use_balance_on_edge_bc "
                    elif [ "$the_case" == "_free" ]; then
                        args=$args" --use_free_bc"
                       # args=" --use_free_bc" #original - uses curviline
                    fi
                    
                   #args=$args" "$boundary_command_line_arg # using this - black box helper message appears
                    
                    echo " "
                    echo " " 
                    echo "Running: "
                    ls -l $executable
                    echo $args
                    echo " in "`pwd`
                    echo " "
                    echo " "
                    
                    # Do it
                    ./$executable $args > OUTPUT
                    
                    
                    oomph-convert mesh_black_box_upgrade.dat
                    oomph-convert mesh_before_black_box_upgrade.dat
                    
                    oomph-convert -p2 duplicated_nodes.dat rotated_nodes.dat 
                    oomph-convert elements_upgraded_to_curved.dat rotated_elements.dat split_elements.dat
                    
                    oomph-convert -p2 boundary_coordinate*.dat
                    oomph-convert test_bulk_elements_on_boundary*.dat
                    oomph-convert -p2 test_face_elements_on_boundary*.dat
                    
                    cd RESLT
                    oomph-convert soln*.dat; makePvd soln soln.pvd 
                    oomph-convert full_soln*.dat; makePvd full_soln full_soln.pvd
                    
                    # Next
                    cd $home_dir
                    
                    # Copy to reslt_dir postprocess programs
                    cp postprocess.cc pitchfork1.cc pitchfork2.cc animation_maker.bash compare_anim.pvsm $the_dir/$reslt_dir
                    
                    # make them executable
                    cd $the_dir/$reslt_dir
                    g++ pitchfork1.cc -o pitchfork1
                    g++ pitchfork2.cc -o pitchfork2
                    g++ postprocess.cc -o postprocess
                    
                    echo "Postprocess..."
                    ./postprocess line1_soln*.dat
                    ./pitchfork1 ./PPDAT/pert*.dat
                    ./pitchfork2 line2_soln*.dat
                    
                    # do executable the animation_maker and run it
                    chmod +xwr animation_maker.bash
                    ./animation_maker.bash    
                    
                    # Back to home
                    cd $home_dir

                done
            done 
        done
    done
done

echo " "
echo " "
echo "Done!"
echo " "
echo " "


exit
