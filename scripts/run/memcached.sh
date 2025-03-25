
current_dir=`pwd`
source ${current_dir}/scripts/check_env.sh
source ${current_dir}/scripts/kernel_config.sh

size=$1
if [ -z "$size" ]; then
    size="large"
fi

echo "--- run memcached ${size} ---"

run_memcached() {
    # memcached_path=${current_dir}/benchmark/memcached
    # memcached_server_exe=${memcached_path}/memcached

    ycsb_exe=${current_dir}/benchmark/ycsb-0.17.0/bin/ycsb
    workload_path=${current_dir}/benchmark/workloada.${size}
    ycsb_args="load memcached -s -P $workload_path -threads 10"

    # run memcached server in backend
    echo "start memcached server in backend"
    # ${memcached_server_exe} &

    # should wait until loading is done
    # or cause error: LOADING memcached is loading the dataset in memory
    # check_memcached_loading
    sleep 5

    run ${ycsb_exe} ${ycsb_args}
    
    # kill -9 `pgrep memcached`
}

run_memcached