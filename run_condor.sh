tar -czf MPSS-source.tar.gz CMakeLists.txt config.hpp.in build_and_run.sh exe inc src config init_vis.mac vis.mac

condor_submit MPSS.submit