current_dir=`pwd`

echo "Compiling gapbs..."
cd ${current_dir}/benchmark/gapbs
make
# pr_exe=${current_dir}/benchmark/gapbs/pr
# pr_args="-u26 -k20 -i10 -n100"

cd ${current_dir}

echo "Compiling graph500..."
cd ${current_dir}/benchmark/graph500/src
make
# graph500_exe=${current_dir}/benchmark/graph500/src/graph500_reference_bfs
# graph500_args="20 16"

cd ${current_dir}

echo "compiling NPB3.0-omp-C..."
cd ${current_dir}/benchmark/NPB3.0-omp-C
make
