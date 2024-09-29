

benchmark_names=("gapbs" "graph500" "NPB3.0-omp-C" "redis" )

# echo with "\t"
echo "preparing benchmark:"
for name in ${benchmark_names[@]}
do
    echo -e "\t$name"
done
exit

# redis_url=https://github.com/redis/redis/archive/refs/tags/7.4.0.zip

# current_dir=`pwd`
# benchmark_dir=${current_dir}/benchmark

# wget $redis_url -O $benchmark_dir/redis.zip
# unzip $benchmark_dir/redis.zip -d $benchmark_dir
# cd $benchmark_dir/redis-7.4.0
# make -j`nproc`

# cd ${current_dir}

# wget  https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/multicore-liblinear/liblinear-multicore-2.47.zip -P ${current_dir}/benchmark
# unzip ${current_dir}/benchmark/liblinear-multicore-2.47.zip -d ${current_dir}/benchmark
# cd ${current_dir}/benchmark/liblinear-multicore-2.47
# patch -p1 < ../../train.diff
# make 
wget https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/HIGGS.xz
xz -d HIGGS.xz

wget https://github.com/brianfrankcooper/YCSB/releases/download/0.17.0/ycsb-0.17.0.tar.gz