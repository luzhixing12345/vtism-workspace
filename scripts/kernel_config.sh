
KERNEL_VERSION=$(uname -r)
LOG_FILE=${current_dir}/output/${KERNEL_VERSION}-benchmark.log

echo "Kernel version: ${KERNEL_VERSION}"
echo "Log file: ${LOG_FILE}"

flush_cache() {

    # 1: clean page cache
    # 2: clean directory entry cache
    # 3: clean page cache, directory entry cache and inode cache
    echo 3 | sudo tee /proc/sys/vm/drop_caches
    echo "flushed cache"
}


log_init() {
    
    # if output directory does not exist, create it
    mkdir -p ${current_dir}/output/
    rm -f ${LOG_FILE}
}