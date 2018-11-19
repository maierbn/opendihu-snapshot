# in workspace (lfs)
git clone https://github.com/maierbn/opendihu-snapshot.git
cd opendihu-snapshot

# load any PrgEnv (works with Cray, GNU, Intel, Cray is preferred for performance, GNU compiles the fastest)
module load cray-petsc/3.7.6.0  # newest petsc version does not work

# build dependencies and opendihu library for the first time
make rebuild_hazelhen

# the next time to build the library use
# make release


# go to folder where example is located
cd examples/electrophysiology/shorten

# build example (links to the built library), takes ~10min if using Cray compiler
python2.7 ../../../dependencies/scons/scons.py

cd build_release

# inspect script.pbs, it simply calls run.sh, look into run.sh, you can replace "scenario-name" by a name
qsub script.pbs 

 
# after the job is complete, it creates build_release/logs/log.csv with one line per run
# to display results:
./show_times.py    # averages all runtimes with same scenario-name in logs/log.csv (could be multiple runs)

# some notes:
# If you want to switch PrgEnv, recompile dependencies with `make rebuild_hazelhen`, then delete core opendihu library with `make purge`, then build `make release`.
#
# `make release` just calls scons in the top folder, you get the same effect with
# python2.7 ../../../dependencies/scons/scons.py BUILD_TYPE=release     # BUILD_TYPE is optional since release is default
#
# The build system is `scons`, it is included in opendihu (in dependencies/scons). All settings to the scons environment can be made in the file `user-variables.scons.py` or by giving them as argument to scons.
# Notable arguments: USE_CRAY_PAT: enable craypat regions in the code (disables measurement of initialization), USE_HPL: add the -hpl option
# Example usage: 
# python2.7 ../../../dependencies/scons/scons.py USE_CRAY_PAT=True USE_HPL=True    # when on PrgEnv-cray, enable cray_pat and -hpl option (but takes ages to compile)
#
# The shorten example builds a kernel in runtime and loads the .so dynamically. Compiler flags can be adjusted in file `opendihu-snapshot/examples/electrophysiology/shorten/settings_shorten_hazelhen.py`, line 295. The compiler is CC.
#
# Using pat_run with GCC, to also include dynamically loaded kernel in tracing:
# 1. needs -finstrument-functions in compile options for all code levels. I.e., core, example code (set USE_CRAY_PAT=True) and eventually in runtime compiled code. (Set compiler options accordingly in settings file.)
# 2. modules needed: PrgEnv-gnu, perftools-base, perftools-preload.
# 3. aprun command for example: ´aprun -n8 pat_run -m lite-events -gmpi -r ./shorten_implicit ../settings_shorten.py´

