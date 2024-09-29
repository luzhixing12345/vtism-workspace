
# benchmark

## gapbs

[gapbs](https://github.com/sbeamer/gapbs)

```bash
git clone git@github.com:sbeamer/gapbs.git
make -j`nproc`
```

## graph500

```bash
git clone https://github.com/Graph500/graph500.git
cd graph500/src
make 
```

```bash
./graph500_reference_bfs 20 16
```

## NPB

[code](https://github.com/benchmark-subsetting/NPB3.0-omp-C)

```bash
git clone git@github.com:benchmark-subsetting/NPB3.0-omp-C.git
```

NAS Parallel Benchmarks（NPB）是一套用来评估超级计算机性能的基准测试套件。它包含多个不同的基准测试程序，每个程序都有其特定的计算模式和数据访问特性。以下是一些基准测试程序及其可能对内存性能（包括延迟和带宽）的影响：

- bt: 密集矩阵操作(used)
- cg: 求解稀疏线性方程组的基准测试(used)



2. **CG (Conjugate Gradient)**：CG是一个求解稀疏线性方程组的基准测试，涉及到稀疏矩阵的运算。它能够揭示出内存带宽和延迟的问题，尤其是在处理大规模稀疏矩阵的情况下。

3. **EP (Embarassingly Parallel)**：这个基准测试程序模拟了可以完全并行化的任务，它主要测试的是处理器的速度而不是内存性能。因此，它不是最适合测试内存性能的。

4. **FT (Fast Fourier Transform)**：FFT涉及到大量的数据重排和变换，这使得它成为测试内存带宽的一个良好选择。此外，由于其计算密集型特性，也可以观察到与内存延迟相关的信息。

5. **IS (Irregular)**：这个基准测试程序模拟了不规则的内存访问模式，比如那些在分子动力学或N体模拟中常见的。它是测试内存延迟的好工具，因为它会频繁地随机访问内存中的不同位置。

6. **LU (LU Decomposition)**：LU分解是另一个涉及密集矩阵操作的基准测试，它同样适用于测试内存带宽。由于存在大量的矩阵乘法和加法，它也可以提供关于内存延迟的信息。

7. **MG (Multigrid)**：MG是一个多网格求解器，用于求解偏微分方程。它涉及到多级数据结构，并且需要大量的数据交换，这使得它成为测试内存带宽和延迟的好工具。

8. **SP (Shellsort with Pointers)**：这个基准测试程序主要关注CPU性能而非内存性能，尽管它也涉及到一些内存访问，但它不是最佳的选择来专门测试内存性能。

综上所述，如果你想要测试内存性能，特别是延迟和带宽，BT、CG、FT、LU 和 MG 都是比较合适的选择。特别是 FT 和 BT，由于它们的数据访问模式以及计算需求，能够更全面地测试内存系统的表现。