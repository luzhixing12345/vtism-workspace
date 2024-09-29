

benchmark_names=("gapbs" "graph500" "NPB3.0-omp-C" "redis" )

# echo with "\t"
echo "preparing benchmark:"
for name in ${benchmark_names[@]}
do
    echo -e "\t$name"
done
exit

redis_url=https://github.com/redis/redis/archive/refs/tags/7.4.0.zip

current_dir=`pwd`
benchmark_dir=${current_dir}/benchmark

wget $redis_url -O $benchmark_dir/redis.zip
unzip $benchmark_dir/redis.zip -d $benchmark_dir
cd $benchmark_dir/redis-7.4.0
make -j`nproc`

cd ${current_dir}