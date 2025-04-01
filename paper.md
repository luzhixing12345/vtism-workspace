
# vtism

## 1 introduction

近年来，人工智能、大数据分析和云服务等内存密集型应用的指数级增长改变了计算领域的格局。这些应用在容量和性能方面都需要大量内存资源。例如，在大规模机器学习训练中，拥有数十亿参数的模型需要大量内存来存储数据和中间结果。虚拟机（VM）已成为在多租户和隔离环境中部署这些应用的热门选择。主要依赖 DRAM 的传统内存系统面临着一些挑战。DRAM 的每比特成本相对较高，这限制了以低成本高效率的方式扩展内存容量。此外，随着应用越来越复杂，DRAM 在容量和性能方面有限的可扩展性也成为瓶颈。例如，在同时运行大量虚拟机的云数据中心，共享 DRAM 资源很快就会饱和，导致性能下降。这导致传统的单层内存系统难以满足这些应用在虚拟化环境中日益增长的需求

为应对这些挑战，多层内存系统已成为一种前景广阔的替代方案。这些系统将不同类型的内存整合到一个分层结构中，每种内存在访问延迟、容量和成本方面都有不同的特点。在低层，快速但昂贵的 DRAM 可为常用数据提供低延迟访问，确保关键操作的快速响应时间。在中层或高层，非易失性内存（NVM）或其他新兴内存技术以较低的每比特成本提供较高的容量，尽管访问延迟较高。这种分层安排可以更有效地利用内存资源，平衡性能与成本，满足不同应用和数据访问模式的特定要求。

在多层内存系统的各种组件中，Compute Express Link（CXL）内存备受关注。CXL 是一种开放式标准互连协议，可实现对远程内存资源的直接和高速缓存一致性访问。该技术在现代计算中具有几个关键优势。首先，它能在 CPU 和内存设备之间提供高带宽通信，这对于需要快速数据传输的应用（如内存数据库和大规模数据处理）至关重要。其次，CXL 内存支持内存池和共享等功能，允许在多个计算节点之间更灵活地分配资源。这在虚拟化环境中尤其有益，因为在这种环境中，多个虚拟机（VM）需要高效地访问和共享内存资源。此外，CXL 的高速缓存一致性机制可确保不同内存层之间的数据一致性，从而降低内存管理的复杂性，提高系统的整体性能[3, 4]。

分层内存管理包括一系列旨在优化不同内存层利用率的技术和策略。页面放置是一个关键环节，它涉及根据每个页面的访问频率和数据特征，为其确定最合适的内存层。例如，访问频率高的热页面通常放置在 DRAM 中，以尽量减少访问延迟，而冷页面则可以迁移到延迟较高、成本较低的内存层。页面迁移是另一个重要的组成部分，可以随着应用访问模式的变化，在不同内存层之间移动页面。这有助于平衡整个内存层的负载，提高系统的整体性能。此外，内存管理还包括缓存管理、内存分配和去分配等考虑因素，以确保有效利用资源。

然而，在虚拟化环境中应用分层内存系统时，仍有几个难题尚未解决。其中一个主要问题是跨多个虚拟机和不同内存层管理高速缓存一致性的复杂性。在虚拟化环境中，多个虚拟机共享物理内存资源，保持缓存一致性变得更加复杂。要确保所有虚拟机对不同内存层中的数据拥有一致的视图，同时又不产生过多的开销，这是一个巨大的挑战。现有的缓存一致性协议往往难以在这种多租户环境中有效扩展，从而导致潜在的数据不一致和性能下降。

另一个挑战在于如何有效处理内存访问延迟差异。不同内存层的不同访问延迟会严重影响虚拟机的性能。由于在虚拟机上运行的应用程序具有不同的内存访问模式，因此很难预测和优化这些延迟变化。例如，一些应用程序从远程内存层访问数据时可能会出现大量缓存未命中，从而导致处理延迟增加。开发能够实时适应这些延迟差异并优化每个虚拟机内存访问的智能算法和技术至关重要，但这仍是一个有待解决的问题。

此外，虚拟化环境的动态特性（虚拟机的数量及其资源需求可能会迅速变化）也给高效的内存访问带来了挑战。

## 2 background and motivation

### 2.1 Tiered Memory Management Solutions

### 2.2 Challenges in Virtualized Environments

现有的分层内存管理系统可以较好的平衡系统中异构内存之间的页面追踪, 分类和迁移, 但是在虚拟化场景下有一些特殊的挑战 因此需要专为虚拟化设计的分层内存管理系统

**虚拟化场景下优化目标的特殊性**: 在异构内存系统中，分层内存管理的核心目标是 最大化利用不同层级内存的容量和性能差异，通过 合理的页面分配和迁移策略 提高整体系统性能。其直接体现为降低进程的访存延迟，提升内存带宽利用率，最终达到缩短进程运行时间 或 提升系统吞吐量的目的

在 虚拟化环境 下，分层内存管理的目标与裸机环境有所不同。大部分云厂商利用 一台物理服务器的 CPU 和内存创建多个虚拟机（VMs），这些虚拟机由 虚拟机管理程序（VMM） 统一管理，为外部提供服务。因此，虚拟化环境的核心挑战在于：VMM 作为中间层，负责管理多个 Guest OS 的内存；同时 Guest OS 本身不直接受 Host OS 管理，而是 VMM 进程中的一个“   子进程”。因此，在虚拟化场景下，分层内存管理的优化目标不应仅仅是提升 VMM 自身的性能，而是最大化 Guest OS 的性能，即确保 Guest OS 运行的应用程序能更好地利用分层内存资源，VMM 需要感知 Guest OS 的访存模式，动态调整内存分配策略

由于 Guest OS 的物理内存（Guest Physical Memory, GPM）并不是真正的物理地址（Host Physical Memory, HPM），而是由 VMM 进行 地址映射和管理，现代 VMM 结合 软件和硬件加速技术 来进行内存虚拟化（例如影子页表（SPT, Shadow Page Table）和嵌套页表（EPT, Extended Page Table / NPT, Nested Page Table））。在 分层内存管理与页面迁移 的过程中，VMM 需要进行 额外的同步，调整 EPT / NPT 映射关系，确保 Guest OS 访问的 GPM 正确映射到合适的 HPM，动态调整 Guest OS 的内存分配，避免热点页面长时间驻留在低速内存中，与 Guest OS 配合进行 NUMA 感知调度，优化跨 NUMA 访存性能

因此在虚拟化环境下，分层内存管理的目标应更侧重于 优化 Guest OS 的内存访问，而非单纯提高 VMM 的性能。因此，在 设计虚拟化感知的分层内存管理方案 时，需要充分考虑：Guest OS 访存行为的感知；VMM 的 EPT/NPT 同步机制；跨 NUMA 和异构内存的动态页面迁移策略

> 传统分层方案未考虑虚拟化层（如KVM/QEMU）导致的语义隔阂，

**虚拟化中页面类型的识别**

在宿主机环境中无法直接获知虚拟机内部的真实页面类型，因为页面类型的判定依赖于 页缓存（Page Cache） 及其管理策略，而这些信息仅存于 Guest OS 的内核。宿主机尝试通过 扫描 VMM 进程的页表 来推断页面类型，但由于 VMM 仅作为 Guest OS 的抽象层，这种方法往往 不准确，因为具体的 页面类型判定和管理策略仍然由 Guest OS 负责。

我们使用 liblinear 进行测试，使用了一个包含 7.4GB HIGGS 数据集的大文件，并在宿主机与虚拟机内部分别统计页面类型，发现了显著差异：

宿主机视角：宿主机将 所有 VMM 进程的页面都归类为匿名页面（Anonymous Pages）；虚拟机视角：虚拟机内部的统计结果显示，存在大量 HIGGS 数据集对应的文件页（File-backed Pages）

![](result/track/host_guest_file_anon/anon_pages_comparison.png)

Linux 内核对 文件页（File-backed Pages） 和 匿名页（Anonymous Pages） 采用 不同的回收策略：文件页 由于有后备存储（如磁盘），通常 更容易被回收，策略也 更激进；匿名页（如应用程序的堆、栈）没有后备存储，回收时需要进行 交换（Swap-out），因此回收更为 保守。

在 虚拟化场景下：宿主机无法识别 Guest OS 的文件页，因为 VMM 进程的所有内存分配在宿主机看来都是匿名页，这导致宿主机和 Guest OS 之间的内存管理策略不匹配，可能影响 内存回收效率，甚至 导致 Guest OS 在需要回收文件页时宿主机错误回收了关键匿名页，因此我们需要一种 Guest OS 与宿主机的协作机制：让 Guest OS 通过协议向宿主机 反馈页面类型

结论：虚拟化环境中的页面类型感知问题 导致 宿主机的内存回收策略不准确，影响 Guest OS 的内存管理效率。这一问题需要通过 VMM 级别的优化、Guest OS 反馈机制以及智能化的内存管理策略 来解决，以 提高虚拟机性能并优化宿主机的内存资源利用。

## vtism design and implementation

### overview

VTISM（Virtualization-aware, NUMA-optimized Tiered Memory System） 是一个专为 虚拟化环境 设计的 分层内存管理系统，它结合 Hypervisor 技术、NUMA 亲和性优化 和 异构内存管理（DRAM + CXL），以 最小化虚拟化开销，实现 高效的页面管理和迁移策略。

VTISM 充分利用 硬件虚拟化特性（如 Intel PML（Page Modification Logging）、EPT（Extended Page Tables））和 软件优化（如 自适应 Guest 页表扫描、增强版 MGLRU（Multi-Generational LRU）页面温度追踪），以 减少 Guest OS 与 Host OS 之间的内存管理语义隔阂，优化 跨 NUMA 低延迟的页面提升（Promotion）与降级（Demotion），确保充分发挥虚拟机在分层内存系统上的性能潜力

VTISM 包含三大核心模块

Adaptive Optimized Guest Page tracking。goal: 以最小的Hypervisor开销追踪虚拟机的内存访问模式，根据虚拟机负载情况动态调整扫描频率，在guest os内部进行页面扫描，对操作系统进程页表进行A/D扫描优化，以最小化扫描开销，提高扫描效率。

MGLRU增强的页面温度分类器：goal:结合Guest OS的提示和Host侧的观测，提升冷/热页面分类精度,优化内存分配策略。通过guest页表扫描获取精确页面访问信息，使用共享内存向宿主机内核暴露guest os进程级工作集信息, Host侧的MGLRU算法整合 Guest 访存语义，PML 访问日志和NUMA局部性，形成更精准的冷热页面分类,提高 回收决策精准度。热页面（高访问频率）优先存入 DRAM，以降低访问延迟。冷页面（低访问频率）迁移到 CXL-DRAM，最大化异构内存的容量优势。

NUMA优化的异步页面迁移器: 目标：在保持NUMA亲和性的前提下最小化迁移延迟。将迁移任务调度到目标虚拟机vCPU所在的NUMA节点。根据NUMA链路延迟带宽负载动态调整迁移方向

### Adaptive Optimized Guest Page tracking

自适应扫描机制：根据 Guest OS 的负载情况 动态调整页表扫描频率，减少 不必要的开销。

进程级 A/D 位扫描优化：在 Guest OS 内部执行页表扫描，减少 Hypervisor 直接干预，提升扫描效率。

redis

而不管 VM 上运行的应用程序的工作集大小如何。因为 EPT 扫描器不知道访问了哪些页面，所以它必须扫描整个 VM 映射来检查页面的 A/D 位，这会导致扫描时间很长

![](./result/track/gpt-vs-ept/gpt-vs-ept.png)


页表扫描的核心目标是检测进程的内存访问行为，这通常通过在两次扫描之间检查页表项的 ACCESS/DIRTY 位（A/D 位）是否被置 1 来实现。具体而言，扫描过程中记录哪些页表项被访问，并在确认访问后清除 A/D 位，同时刷新对应的 TLB（Translation Lookaside Buffer）中的缓存项。这一机制不可避免地会引入额外的开销，主要体现在增加访存延迟和影响程序性能。

我们的分析表明，进程的活跃页表项数量与扫描所需时间呈正相关关系。扫描时间越长，意味着当前系统内存访问负载较高。如果仍然采用固定时间间隔进行扫描，可能会导致不必要的额外开销，尤其是在高负载情况下进一步加剧性能损耗。

![](./result/track/scan_time/scan_time.png)

在实验中，我们编写了一个测试程序，该程序分配 16GB 内存并进行持续的随机访存，并记录在1s内的访存次数，以此来评估启用内核模块进行后台页表扫描对系统性能的影响。实验中设置了不同的扫描间隔，分别为 1000ms、2000ms、4000ms 和 6000ms，旨在观察不同扫描频率下对访存性能的影响。

结果表明，后台进行页表扫描显著影响了程序的访存性能，可以看出扫描间隔越短对系统访存性能的影响越大，并且在扫描发生时系统的访存性能下降最为明显。当扫描间隔设置为 6000ms 时，即每 6 秒进行一次后台扫描，此时系统的访存次数显著减少。这是因为后台扫描在消耗 CPU 资源的同时，清除了 TLB 中的页表项缓存，从而导致每次访存时必须重新访问内存，进一步加重了内存访问的延迟。扫描结束后随着时间推移，系统的访存性能逐渐恢复到基线（baseline）水平。

通过这种测试，我们可以看出，启用后台页表扫描会影响程序的访存效率，尤其是在高频扫描的情况下。随着扫描频率的增加，系统资源被更多地分配给页表扫描过程，导致正常的内存访问延迟增加，最终影响整体性能。因此，在设计内存管理和页表扫描策略时，需要平衡性能需求和扫描频率，以避免过度干扰系统的正常运行。

![](./result/track/pt_scan_impact/memory_accesses.png)

我们提出了一种动态调整扫描间隔的方法，使其能够根据实际的内存访问活跃程度进行自适应调整。

具体地，我们通过如下公式来计算扫描间隔 T_interval：

T_interval = (SCAN_K * T_scan + SCAN_K_BASE) * T_scan + SCAN_BASE_INTERVAL

t = (ax+b)x + c

其中：  
- T_scan 代表当前扫描所消耗的时间  
- SCAN_K 和 SCAN_K_BASE 是调节系数，用于控制扫描间隔对扫描时间的敏感度  
- SCAN_BASE_INTERVAL 是基础扫描间隔，保证低负载情况下仍然能进行周期性扫描  

该动态调整策略能够根据系统负载适应性地延长或缩短扫描间隔，降低对程序性能的影响，同时保持对进程访存行为的有效监控。

### MGLRU-based Page Classification

### NUMA-aware Multi-thread Asynchronous Page Migration

affinity

Linux 的页面迁移有同步迁移和异步迁移两种模式，同步迁移先尝试使用MIGRATE_ASYNC模式批量迁移folios

如果批量迁移失败,则退回到逐个同步迁移每个folio [code](https://github.com/torvalds/linux/blob/ffc253263a1375a65fa6c9f62a893e9767fbebfa/mm/migrate.c#L1825)。

但是我们注意到现在 linux 内核使用的异步迁移不是真正的异步操作，依然是同步顺序的 copy 页面并在循环中调用 cond_resched[code](https://github.com/torvalds/linux/blob/ffc253263a1375a65fa6c9f62a893e9767fbebfa/mm/util.c#L790C1-L801C2)

多线程异步迁移，针对 页面提升（hot page migration） 和 页面降级（cold page demotion） 进行优化。思路是使用 工作队列（workqueue） 进行异步提交，并在后台完成页面迁移。

在内核中，页面迁移（Page Migration）发生的原因多种多样，不同的迁移场景对执行顺序和阻塞行为有不同的要求。其中，一些页面迁移操作需要保持严格的执行顺序，因此必须同步进行并阻塞相关进程。例如内存规整（MR_COMPACTION），内存热插拔（MR_MEMORY_HOTPLUG）系统调用（MR_SYSCALL），调用 mbind 系统调用设置 memory policy 时触发的迁移（MR_MEMPOLICY_MBIND）等

另一方面，某些迁移类型主要用于内核在系统层面优化内存访问模式，因而无需同步执行，可以采用异步机制在后台完成，以减少对前台任务的影响。例如页面提升（MR_NUMA_MISPLACED）和页面降级 （MR_DEMOTION）. 对于这些无需严格同步的迁移任务，内核可以采用异步方式进行后台迁移，以避免影响前台应用程序的执行效率。这种异步迁移策略可以提高系统的整体性能，同时保证内存资源的高效利用。

页面提升和页面降级的特性不同，因此可以采用 不同的队列调度策略

页面提升的目标是快速迁移高频访问的页到更快的存储（如 DRAM），先检查 目标 NUMA node 是否有足够的可用内存，避免迁移后导致回退。


页面降级的目标是迁移低访问频率的页面到较慢的存储。 低优先级的后台线程可以在内存压力较低时异步迁移，避免影响前台进程性能，采用 时间窗口（time window）+ 访问统计，避免短期冷热变化导致频繁迁移（MGLRU）

采用提升和降级双工作队列，不同的优先级任务交给不同的 worker。高优先级队列：处理页面提升，尽快将热点页面迁移到 DRAM，设置WQ_HIGHPRI：让热页迁移尽快完成，减少性能抖动；低优先级队列：处理页面降级，在 内存压力较低 时进行，设置WQ_UNBOUND：让调度更灵活，充分利用多核 CPU 资源。

减少锁争用：使用 RCU 避免页表锁，避免高并发时 mmap_lock 带来的性能瓶颈；冷页迁移可以使用 trylock，减少锁等待对应用程序的影响

预取优化：页面提升前，可以先检查目标地址是否存在 TLB entry，如果不存在，提前 prefetch 目标页面（prefetchw(new_page);）可以迁移后新页面的 TLB miss，提升访问速度。

活跃进程对其活跃页面的访问是影响系统性能的关键因素。如果这些活跃页面位于慢速内存节点（如 CXL-DRAM 或 NVM），我们希望尽快将其迁移到高速内存节点（如 DRAM），以降低访问延迟。然而，页面迁移的前提是目标节点具备足够的可用内存，否则迁移操作将会失败，从而影响性能优化的效果。因此，在执行页面迁移时，确保目标节点的可用内存充足是至关重要的。

为了解决这一问题，内核采用 空闲页面回收水位线（Watermark for Page Reclaim） 机制来决定何时启动页面回收。当系统空闲内存下降到设定的水位线以下时，内核会触发 页面降级（Page Demotion），即将 DRAM 中的冷页面迁移到 CXL-DRAM 或 NVM 等较慢的存储介质，以释放更多的 DRAM 资源。这一机制确保了当高优先级进程需要更多高速内存时，系统可以及时提供可用的 DRAM 页面，进而提高页面提升（Page Promotion）的成功率和效率。

由于内存回收和页面降级涉及大量的数据迁移，其执行需要一定的时间。因此，系统必须提前预留足够的空闲内存空间，以保障后续页面提升操作能够顺利进行。换句话说，提高 页面提升的成功率和执行效率 是提升系统性能的核心目标，而更主动地进行 提前页面降级 则是实现这一目标的重要保障。通过合理调整页面回收策略，使降级与提升协同工作，可以有效优化内存资源的分配，提高整体系统性能。

将内存页面从源位置迁移到目的位置需要四步：
（1）在目标节点中分配新页面
 (2) 取消映射要迁移的页面（包括使PTE失效）
 (3) 将页面从源节点复制到目标节点
（4）映射新页面（包括更新PTE）

> (1) allocate new pages in the target node; (2) unmap pages to migrate (including invalidating PTE); (3) copy pages from source to target node; (4) map new pages (including updating PTE)

每个节点一个 promote_wq demote_wq, wq numa node CPU 亲和性

<!-- https://docs.kernel.org/core-api/workqueue.html -->

promote_wq[dst_node]: 页面提升时，任务提交到 dst 所在 NUMA 节点的 promote_wq

demote_wq[src_node]: 页面降级时，任务提交到 src 所在 NUMA 节点的 demote_wq

nomad 只使用一个工作队列 [code](https://github.com/lingfenghsiang/Nomad/blob/602da42dcd6b6cf86ecfa50ba1ab7ab3b6dceb9f/src/nomad_module/async_promote_main.c#L1080)

## evaluation and analysis

### Experimental Setup

| **Component**  | **Specification**  |
|---------------|------------------|
| **CPU**       | 2× Intel® Xeon® Platinum 8468V CPUs @3.8 GHz, 48 cores per socket, 192 total CPUs, Hyper-Threading disabled |
| **Memory**    | **NUMA Node 0 & 1:** DDR5-4800, 32GB per channel, Hynix DIMM modules <br> **NUMA Node 2 & 3:** CXL-DRAM, 64GB, Hynix DIMM modules |
| **NUMA Nodes** | Node 0: CPU 0-47, 32GB DDR5-4800 <br> Node 1: CPU 48-95, 32GB DDR5-4800 <br> Node 2: CXL-DRAM, 64GB <br> Node 3: CXL-DRAM, 64GB |
| **Virtualization** | VT-x support |         

- autonuma 
- tpp
- nomad

Hemem PEBS 虚拟化不合适

| **Benchmark**  | **Category**                         | **RSS**  |
|---------------|--------------------------------------|---------|
| Redis (YCSB)  | Key-Value Store                     | 29 GB   |
| PR            | Graph Processing                    | 21 GB   |
| Graph500      | Graph Processing                    | 15 GB   |
| XSBench       | High-Performance Computing         | 14 GB   |
<!-- | Btree         | Measures index lookup performance  | 16 GB   |
| GUPS          | Random Memory Access Patterns      | 16 GB   | -->

### page tracking

overhead 测试了 500 1000 2000 4000

- SCAN_K = 0.5
- SCAN_K_BASE = 50
- SCAN_BASE_INTERVAL = 2000

2000+ 

![](./result/track/overhead/overhead.png)

优化的页表树扫描：在 Linux 内核中，针对进程页面的遍历通常使用 walk_page_vma 进行单个 VMA（虚拟内存区域）的页表遍历，而 walk_page_range 可以用于访问整个进程的地址空间。然而，传统的页表扫描方法存在性能瓶颈，特别是在大规模进程内存管理时。Linux 采用 四级/五级页表 结构进行地址翻译，页表访问的层次包括：PGD（Page Global Directory）PUD（Page Upper Directory）PMD（Page Middle Directory）PTE（Page Table Entry）（P4D，部分体系结构使用五级页表）

在进行一次完整的地址翻译时，硬件会依次访问每一层的页表项，并且 每一级的页表项都会被设置 ACCESS 位。这意味着如果某个 PTE 级别的页表项被访问，那么它的所有上级页表节点（PMD/PUD/PGD）的 ACCESS 位也会被置位。

基于这一观察，我们可以优化页表扫描的范围：

逐层检查 ACCESS 位：如果一个 父页表节点（PGD/PUD/PMD） 的 ACCESS 位未被设置，则可以直接跳过其所有子节点的扫描，因为它们必然未被访问。

减少不必要的 PTE 级别扫描：传统 walk_page_range 可能会访问整个进程的地址空间，而在许多情况下，未被访问的页表区域无需遍历。

借鉴 telescope 设计思路：telescope 通过优化 DAMON（Data Access Monitoring）采样来提高访存监控效率，我们借鉴其思想，优化页表扫描逻辑，使其适用于 更普适的页表扫描场景，不仅限于 DAMON 采样。

![](./result/track/pt_scan/pt_scan_redis_bar.png)

redis xsbench pr 都是访存比较密集的应用， graph500 的提升特别明显， 因为 Graph500 主要用于评测大规模图处理的性能，它的核心算法是 BFS（Breadth-First Search，广度优先搜索），Graph500 需要遍历大型图的数据结构（通常是邻接表或压缩格式存储），访问模式呈非连续、非局部特性, 这导致其访存模式是高度稀疏的

opt 的扫描模式会去除掉根节点的访问，大幅缩小扫描范围

![](./result/track/pt_scan/pt_scan_opt_all.png)

### page migration

CPU 绑定到 node0, 初始在 node2 上分配 2GB 内存，读写比例 7:3，不断随机访存 10000 次记录当前页面在 NUMA 节点上的分布情况

对比页面提升速度：页面提升由 numa_hint_fault 驱动，所有分层内存管理算法都在同一时间触发并开始进行 node2->node0 的页面提升, vtism 的页面提升速度是最快的

每个有 CPU 的顶层内存节点设置 4 个迁移线程，并且设置一对 wait_queue

![](./result/migration/promote/promote.png)

```bash
numactl --cpubind=0 --membind=2 ./mm-mem/bin/cpu_loaded_latency
```

```bash
cat /proc/vmstat | grep numa
taskset -cp $(pidof cpu_loaded_latency)
numastat -p $(pidof cpu_loaded_latency)
```

## conclusion and future work

