
# vtism-workspace

workspace repo for <vtism: Toward Efficient Tiered Memory Management for Virtual Machines with CXL>, the source kernel is in [vtism](https://github.com/luzhixing12345/vtism)

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

6 benchmarks in benchmark/

```bash
sudo apt install mpich
```

## scripts

## build pt_scan

```bash
cd pt_scan
make
```

