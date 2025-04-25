
# vtism-workspace

workspace repo for <vtism: Toward Efficient Tiered Memory Management for Virtual Machines with CXL>, the source kernel is in [vtism](https://anonymous.4open.science/r/vtism-kernel)

```bash
Makefile         # qemu start Makefile
README.md        # README
benchmark/       # all test benchmarks
cp.sh*           # a small script for copy files from host disk to guest disk
disk/            # qemu disk dir
get_kernel.sh*   # select a specific kernel(used with Makefile)
kernel/          # (no used, only contains nomad async_promote.ko)
paper.md         # my paper
patches/         # patch files
pt_scan/         # guest page table scan
result/          # all the experiment results, logs, draw scripts and test programs
scripts/         # some useful scripts
userspace/       # mem_stress and wramup program code
```

## benchmark

we use 6 benchmarks under benchmark/

- gapbs
- graph500
- gups_bench
- liblinear-multicore-2.47
- redis-7.4.0
- xsbench

cd benchmark/ and build them, you don't need to directly run them and we will run all by scripts/

## scripts

please prebuild and install vtism kernel first, use [switch_kernel.sh](./scripts/kernel/switch_kernel.sh), enter the number for the kernel you want to switch to

build and install other kernels by your like

```bash
Available kernel versions:
[0]: 4.11.0-041100-generic
[1]: 5.13.0-rc6nomad
[2]: 5.13.0-rc6tpp
[3]: 5.13.1autonuma
[4]: 5.15.19-htmm
[5]: 5.3.0-autotiering
[6]: 5.6.0-rc6nimble+
[7]: 6.4.0hydra+
[8]: 6.6.0autonuma+
[9]: 6.6.0damon+
[10]: 6.6.0vtism+ ← (current)
[11]: 6.8.0-57-generic

Enter the number of the kernel version you want to switch to:
```

the scripts under /scripts/run is used to start all the test

**all the scripts should run in workspace/**, like `./scripts/run/pr.sh`, after you have built the corresponding benchmark program

```bash
$ ls scripts/run/ -l
bc.sh
btree.sh
graph500.sh
gups.sh
liblinear.sh
memcached.sh
pr.sh
redis.sh
xsbench.sh
```

## build pt_scan

pt_scan/ includes the guest page table scan module, you should first add our patch [walk_page_vma_opt.patch](./patches/walk_page_vma_opt.patch) to linux v6.6

```bash
patch -p1 < walk_page_vma_opt.patch
```

compile the guest os kernel and use that kernel to build pt_scan module

```bash
cd pt_scan
make kernel_dir=PATH/OF/YOUR/KERNEL
```

after finished you will get `pt_scan.ko`

there are some useful scripts for batch compilation like [rename.sh](./pt_scan/rename.sh) and [scan_k_rename.sh](./pt_scan/scan_k_rename.sh), which will be used in our test(see our paper)

## start qemu vm

prepare qemu-system-x86_64 and a disk, see Makefile `vm0` and change your $(BZIMAGE) and $(DISK) path

```bash
make
```

it will use `/dev/shm/my_shm` to setup shared memory between guest and host

## result

all the experiment results are under result/

each sub dir has a python file to draw, just run by

```bash
# cd to one dir
python main.py
```

there's also a `run.sh` file under each sub dir, it's used to start test

```bash
~/workspace/result$ tree -d
.
|-- ablation
|-- benchmark
|   `-- output
|-- migration
|   |-- double_promote
|   |-- latency
|   |-- promote
|   `-- rw
`-- track
    |-- adaptive_scan
    |   |-- 10
    |   |-- 6
    |   |-- 7
    |   |-- 8
    |   |-- 8+50
    |   `-- 9
    |-- base_interval
    |-- gpt-vs-hpt
    |-- host_guest_file_anon
    |-- linear_not_good
    |   |-- 100
    |   |-- 30
    |   |-- 40
    |   |-- 50
    |   |-- 60
    |   `-- 80
    |-- overhead
    |-- pt_scan
    |-- pt_scan_impact
    |-- scan_time
    `-- xx_proof
        |-- 1000
        |-- 1500
        |-- 2000
        |-- 2500
        |-- 4000
        `-- 6000
```

## setup system

```bash
sudo ./scripts/setup_system.sh
```