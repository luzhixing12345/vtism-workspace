
current_dir=`pwd`
source ${current_dir}/scripts/check_env.sh
source ${current_dir}/scripts/kernel_config.sh

size=$1
if [ -z "$size" ]; then
    size="huge"
fi

echo "--- run redis ${size} ---"

run_redis() {
    redis_path=${current_dir}/benchmark/redis-7.4.0
    redis_server_exe=${redis_path}/src/redis-server
    redis_server_args=${redis_path}/redis.conf

    ycsb_exe=${current_dir}/benchmark/ycsb-0.17.0/bin/ycsb
    workload_path=${current_dir}/benchmark/workloada.${size}
    ycsb_args="load redis -s -P $workload_path -threads 10 -p redis.host=localhost -p redis.port=6379 -p redis.timeout=3600000"

    # run redis server in backend
    echo "start redis server in backend"
    ${redis_server_exe} ${redis_server_args} &

    # should wait until loading is done
    # or cause error: LOADING Redis is loading the dataset in memory
    # check_redis_loading
    sleep 5

    run ${ycsb_exe} ${ycsb_args}
    
    kill -9 `pgrep redis`
}

run_redis

# RSS: 95.9GB
# CPU: low cpu usage