
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

### 2.3 Opportunities in Virtualized Environments

现代虚拟化环境为分层内存管理提供了独特的优化机会，这主要得益于硬件虚拟化扩展（如Intel EPT、PML）和KVM虚拟化平台的开源特性。这些技术特性共同构成了在虚拟化环境中实现高效内存分层管理的基础。

(1) KVM的模块化与可扩展性

KVM作为基于Linux内核的Type-1 Hypervisor实现，其最大的优势在于与Host OS的深度集成。这种架构设计使得KVM能够直接利用Linux内核成熟的内存管理子系统，包括但不限于MGLRU页面置换算法、NUMA感知的内存调度机制等。更重要的是，虚拟化环境特有的内存共享特性为Guest和Host之间的协作提供了天然优势。与传统的跨OS通信需要通过网络协议栈不同，Guest OS与Host OS共享相同的物理内存空间，这使得二者可以通过建立共享内存区域来高效传递内存访问模式等关键信息，完全避免了网络传输带来的额外开销。这种独特的内存共享机制为实现低延迟的Guest-Host协同内存管理奠定了坚实基础。

(2) 嵌套页表（EPT/NPT）的2D映射优势

现代处理器提供硬件虚拟化 EPT/NPT（Nested Page Table）机制实现了Guest物理地址（GPA）到Host物理地址（HPA）的二维映射架构，这为虚拟化环境中的内存管理提供了独特优势。在这种架构下，Hypervisor可以完全独立地控制GPA到HPA的映射关系，而无需Guest OS的参与或感知。具体来说，当需要进行页面迁移时，Hypervisor只需修改EPT中的映射关系，将GPA重新映射到不同的HPA物理页面即可。这种机制特别适合分层内存管理场景，例如可以将被识别为"冷"页面的GPA映射到慢速内存（如CXL）的HPA，而将"热"页面保持在DRAM中。此外，EPT对大页（2MB/1GB）的支持进一步提升了该机制的实际效益，通过减少TLB失效和页表遍历次数，显著降低了内存访问开销

(3) 多租户环境中的内存利用率优化
在云计算环境中，运行虚拟机的服务器通常只运行多个虚拟机，而不会运行其他的本地服务。每个虚拟机的内存和 CPU 资源分配相对有限，例如 8 vCPUs 和 32GB 内存（及以下配置）为主流。因此，单个虚拟机通常无法充分利用物理主机上的所有计算和内存资源。

研究表明 [16]，在 Azure 的典型计算集群中：40% 的虚拟机使用不超过 2 个 vCPU，86% 的虚拟机使用不超过 8 个 vCPU。

这意味着现代云主机通常需要同时运行数十个虚拟机，但大多数虚拟机的实际负载较低，导致其分配的内存资源可能会出现闲置。传统的 Hypervisor 资源管理策略主要依赖于静态或准动态的内存分配，难以实时适应虚拟机的内存需求波动。

然而，在分层内存管理框架下，可以基于虚拟机的实时内存访问模式进行动态优化：对于长时间未被访问的冷数据，可将其降级到 CXL DRAM 或 SSD Swap，以释放高性能 DDR DRAM 资源。



## 3 vtism design and implementation

### 3.1 overview

VTISM（Virtualization-aware, NUMA-optimized Tiered Memory System） 是一个专为 虚拟化环境 设计的 分层内存管理系统，它结合 Hypervisor 技术、NUMA 亲和性优化 和 异构内存管理（DRAM + CXL），以 最小化虚拟化开销，实现 高效的页面管理和迁移策略。

VTISM 充分利用 硬件虚拟化特性（如 Intel PML（Page Modification Logging）、EPT（Extended Page Tables））和 软件优化（如 自适应 Guest 页表扫描、增强版 MGLRU（Multi-Generational LRU）页面温度追踪），以 减少 Guest OS 与 Host OS 之间的内存管理语义隔阂，优化 跨 NUMA 低延迟的页面提升（Promotion）与降级（Demotion），确保充分发挥虚拟机在分层内存系统上的性能潜力

VTISM 包含三大核心模块

Adaptive Optimized Guest Page tracking。goal: 以最小的Hypervisor开销追踪虚拟机的内存访问模式，根据虚拟机负载情况动态调整扫描频率，在guest os内部进行页面扫描，对操作系统进程页表进行A/D扫描优化，以最小化扫描开销，提高扫描效率。

MGLRU增强的页面温度分类器：goal:结合Guest OS的提示和Host侧的观测，提升冷/热页面分类精度,优化内存分配策略。通过guest页表扫描获取精确页面访问信息，使用共享内存向宿主机内核暴露guest os进程级工作负载信息, Host侧的MGLRU算法整合 Guest 访存语义，PML 访问日志和NUMA局部性，形成更精准的冷热页面分类,提高 回收决策精准度。热页面（高访问频率）优先存入 DRAM，以降低访问延迟。冷页面（低访问频率）迁移到 CXL-DRAM，最大化异构内存的容量优势。

NUMA优化的异步页面迁移器: 目标：在保持NUMA亲和性的前提下最小化迁移延迟。将迁移任务调度到目标虚拟机vCPU所在的NUMA节点。根据NUMA链路延迟带宽负载动态调整迁移方向

### 3.2 Adaptive Optimized Guest Page tracking

#### 3.2.1 guest page table tracking

页表扫描的核心目标是检测进程的内存访问行为，这通常通过在两次扫描之间检查页表项的 ACCESS/DIRTY 位（A/D 位）是否被置 1 来实现。具体而言，扫描过程中记录哪些页表项被访问，并在确认访问后清除 A/D 位，同时刷新对应的 TLB 中的缓存项。这一机制不可避免地会引入额外的开销，主要体现在增加访存延迟和影响程序性能。

在虚拟化环境下在宿主机中传统的页表扫描会带来额外的开销，主要是因为 (1) 宿主机没有能力跨越 VMM 中间层感知 guest VM 上实际运行的工作负载大小。因此宿主机需要扫描整个 VM 的 全部映射内存以判断页面访问情况，导致扫描时间过长 （2）VMM 采用 两级地址转换（EPT/NPT），即 Guest 虚拟地址（GVA）→ Guest 物理地址（GPA）→ Host 物理地址（HPA），重新设置 A/D 位和刷新 TLB 会显著降低 guest os 内存访问速度，带来昂贵的访存开销

VTISM 采用 Guest Page Table（GPT）扫描，在 Guest OS 内部嵌入一个内核扫描模块，替代传统的宿主机 HPT（Host Page Table）扫描方式。在 guest os 中嵌入内核模块扫描页表的好处在于 Guest OS 具备对自身进程的完整感知能力，可以精准扫描实际使用的内存页，而无需扫描整个 VM 的全部映射内存。

#### 3.2.2 Adaptive scan interval

传统的页表扫描选择采用固定扫描间隔，通过实验测试不同 benchmark 负载，确保在加入后台扫描模块后，前台进程性能损失不超过 5%，进而确定最终的扫描间隔。我们认为这种固定间隔扫描方式存在显著的局限性，因为扫描间隔的设置是为了缓解由于页表扫描带来的性能开销，扫描时间与进程的活跃页表项数量直接相关，而活跃页表项数量取决于进程的内存访问模式和工作负载大小。单一固定扫描间隔难以同时兼顾不同类型的应用：
对于 小内存应用，可能不会成为性能瓶颈，导致资源浪费。对于 大内存应用，固定扫描间隔可能引入过高的额外开销，尤其在高负载场景下加剧性能损失。

vtism 提出了一种动态调整扫描间隔的方法，使其能够根据实际的内存访问活跃程度进行自适应调整。

具体地，我们通过如下公式来计算扫描间隔 T_interval：

T_interval = (SCAN_K * T_scan + SCAN_K_BASE) * T_scan + SCAN_BASE_INTERVAL

t = (ax+b)x + c

其中：  
- T_scan 代表本轮扫描所消耗的时间  
- SCAN_K 和 SCAN_K_BASE 是调节系数，用于控制扫描间隔对扫描时间的敏感度  
- SCAN_BASE_INTERVAL 是基础扫描间隔，保证低负载情况下仍然能进行周期性扫描  

扫描时间 Tscan 越长，下一轮扫描间隔 Tinterval 也相应增长，减少扫描对系统的影响。若扫描时间较短，则缩短扫描间隔，保证高效监控进程的访存行为

选择二次函数而非线性函数是因为我们认为后台的扫描时间 T_scan 对前台应用性能的影响会随着 T_scan 的增长成倍提升，因此使用二次方提高扫描间隔缓解由于扫描带来的性能开销。

该动态调整策略能够自适应系统内存负载，平衡不同规模应用的影响，确保小内存应用不过度扫描，大内存应用不过度占用 CPU 资源。

下一轮扫描间隔与本轮扫描间隔有关 T_scan, 可以根据系统负载适应性地延长或缩短扫描间隔，降低对程序性能的影响，同时保持对进程访存行为的有效监控。自适应扫描间隔能够动态适配不同应用的内存访问模式，提高页表扫描的效率，同时降低对前台进程的性能影响。

实验结果见 [4]

#### 3.2.3 optimized page table tree search

Linux 采用四级/五级页表结构进行地址翻译，页表访问的层次包括：PGD（Page Global Directory）、PUD（Page Upper Directory）、PMD（Page Middle Directory）和 PTE（Page Table Entry）（部分架构在此基础上增加 P4D 形成五级页表）。在内核中，针对进程页面的遍历通常使用 walk_page_vma 进行单个 VMA（虚拟内存区域）的页表遍历，或者使用 walk_page_range 访问整个进程的某一段地址空间。目前，大多数依赖页表扫描的系统（如 AutoNUMA、DAMON）直接使用 walk_page_range 遍历所有 PTE，以检查 A/D 位的设置情况。然而，这种方法存在显著的性能瓶颈，尤其是在 大规模内存环境 和 稀疏页表 场景下。

在 x86 以及其他支持 A/D 位的体系架构中，处理器在完成地址翻译时，会依次访问每一级页表项（PGD → PUD → PMD → PTE），并自动设置对应的 ACCESS 位（A 位）。这意味着如果某个 PTE 级别的页表项被访问，那么它的所有上级页表节点（PGD/PUD/PMD）的 A 位也会被置 1。因此，如果在扫描过程中发现某个 父级页表节点的 A 位未被设置，可以直接跳过其所有子页表项的扫描，因为这些页表项必然未被访问。这一优化思路可以 大幅减少 PTE 级别的遍历，避免不必要的页表扫描，降低 CPU 负担。

基于这一观察，我们设计并实现了 优化版页表扫描函数 walk_page_vma_opt，通过 逐层检查 ACCESS 位 来 剪枝未被访问的页表节点，减少 PTE 级别的遍历开销。使用 walk_page_range 会访问整个进程的地址空间，而在许多情况下，未被访问的页表区域无需遍历。linux 内核 API 没有采用这个优化是因为 walk_page_range 的设计目的就是遍历进程的所有页表空间并进行对应的处理，这个优化操作是利用处理器硬件特性并专门针对页面A/D位扫描需求实现的. 该patch仅涉及到一个新文件的添加，可以被无缝应用到几乎大部分内核版本中

我们对比了 telescope 和 MGLRU 中的类似优化方案：

Telescope 主要用于优化 DAMON 采样，提高访存监控效率，而 walk_page_vma_opt 具备更广泛的适用性，保持了和原API相同的接口, 能够无缝适配 autonuma、DAMON 等所有依赖 A/D 位扫描的方案；

MGLRU 采用类似的 逐层扫描优化逻辑，因此我们选择在 Host OS 端结合 MGLRU 进行冷热页分类 能够复用这部分逻辑

优化实验结果见 [4]

### 3.3 MGLRU-based Page Classification

### 3.4 延迟感知的 Multi-thread Asynchronous Page Migration

页面迁移的核心任务是在保证系统整体性能的前提下，准确高效地实现内存页面在不同NUMA节点间的迁移。为实现这一目标，VTISM设计了延迟带宽感知的多线程异步迁移框架

#### 3.4.1 延迟感知的迁移决策

在异构内存架构中，传统的分层内存管理系统通常基于静态的性能假设进行设计，即简单地认为DRAM节点的访问性能在所有场景下都优于CXL内存节点。然而，这种假设忽略了现代计算系统中内存访问行为的动态特性，特别是在高并发、多租户的虚拟化环境中。VTISM通过深入分析内存子系统的实际运行特征，提出了更精细化的迁移决策机制。

DRAM节点的访问延迟并非固定不变，而是会随着本地内存访问频率的增加呈现显著的非线性增长：实验测试了 1-32GB 工作负载持续访存，1channel 32GB DDR DRAM 访问延迟变化曲线。33ns->600ns, 本地 CXL DRAM 访存延迟 288 ns，远程 CXL DRAM 访存延迟 486 ns，随着工作负载的增加 DDR DRAM 的延迟迅速超过了 CXL DRAM 空载的延迟。本地不一定比远端好

![](./result/migration/latency/dram_latency.png)

VTISM采用智能化的迁移决策机制，在进行页面迁移前会对目标NUMA节点的访问特性进行综合评估。具体而言，系统会计算以下关键指标：

远程访问延迟代价：通过对比本地访问延迟与跨NUMA节点访问延迟，如果由于本地的高频访问导致远程访问延迟低于本地访问，那么此时并不

#### 3.4.2 实时延迟获取

UNC_CHA_TOR_OCCUPANCY.IA

UNC_CHA_TOR_INSERTS.IA

#### 3.4.3 异步多线程

Linux 的页面迁移有同步迁移和异步迁移两种模式，同步迁移先尝试使用MIGRATE_ASYNC模式批量迁移folios

如果批量迁移失败,则退回到逐个同步迁移每个folio [code](https://github.com/torvalds/linux/blob/ffc253263a1375a65fa6c9f62a893e9767fbebfa/mm/migrate.c#L1825)。

但是我们注意到现在 linux 内核使用的异步迁移不是真正的异步操作，依然是同步顺序的 copy 页面并在循环中调用 cond_resched[code](https://github.com/torvalds/linux/blob/ffc253263a1375a65fa6c9f62a893e9767fbebfa/mm/util.c#L790C1-L801C2)

多线程异步迁移，针对 页面提升（hot page migration） 和 页面降级（cold page demotion） 进行优化。思路是使用 waitqueue 进行异步提交，并在后台完成页面迁移。

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

demote_wq[src_node]: 页面降级时，任务提交到 dst 所在 NUMA 节点的 demote_wq

需要一个理由

nomad 只使用一个工作队列 [code](https://github.com/lingfenghsiang/Nomad/blob/602da42dcd6b6cf86ecfa50ba1ab7ab3b6dceb9f/src/nomad_module/async_promote_main.c#L1080)

## 4 evaluation and analysis

### 4.1 Experimental Setup

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

### 4.2 Experimental Approach

### 4.3 page tracking

#### 4.3.1 gpt-hpt

我们在一台 配置 32GB 内存的 VM 上运行 Redis + YCSB 负载，评估了 HPT 扫描（Host 侧扫描）与 GPT 扫描（Guest 侧扫描）在扫描范围上的差异。运行前对 VM 进行预热，建立完整的 32GB 内存映射. 实验结果表明 由于宿主机无法感知 Guest OS 进程的工作负载情况，始终扫描完整的 32GB 内存范围。Guest OS 直接感知 进程实际使用的内存区域，仅扫描真实工作负载，大大减少了要扫描的页面数量

![](./result/track/gpt-vs-hpt/gpt-vs-hpt.png)

#### 4.3.2 static scan interval impact for vm

页表扫描过程中重新设置 A/D 位和刷新 TLB 会显著降低 guest os 内存访问速度，在进行两级地址转换的虚拟化环境中带来的开销尤为明显。在实验中，我们编写了一个测试程序，该程序分配 16GB 内存并进行持续的随机访存，并记录1s内的总访存次数，以此来评估启用内核模块进行后台页表扫描对系统性能的影响。实验中设置了不同的扫描间隔，分别为 1000ms、2000ms、4000ms 和 6000ms，旨在观察不同扫描频率下对访存性能的影响。

扫描时间相同

我们将测试程序和页表扫描模块在虚拟机中运行，结果表明，后台进行页表扫描显著影响了程序的访存性能，可以看出扫描间隔越短对系统访存性能的影响越大，并且在扫描发生时系统的访存性能下降最为明显。当扫描间隔设置为 6000ms 时，即每 6 秒进行一次后台扫描，此时系统的访存次数显著减少。这是因为后台扫描在消耗 CPU 资源的同时，清除了 TLB 中的页表项缓存，从而导致每次访存时必须重新访问内存，进一步加重了内存访问的延迟。扫描结束后随着时间推移，系统的访存性能逐渐恢复到基线（baseline）水平。

通过这种测试，我们可以看出，启用后台页表扫描会影响程序的访存效率，尤其是在高频扫描的情况下。随着扫描频率的增加，系统资源被更多地分配给页表扫描过程，导致正常的内存访问延迟增加，最终影响整体性能。因此，在设计内存管理和页表扫描策略时，需要平衡性能需求和扫描频率，以避免过度干扰系统的正常运行。

![](./result/track/pt_scan_impact/memory_accesses.png)

为了验证进程运行时其活跃页表项和后台扫描时间的关系，我们在虚拟机中运行了 gabps pr，并记录后台页表扫描时的活跃页表项和扫描时间的变化曲线，结果表明，进程的活跃页表项数量与扫描时间呈正相关关系。当前系统内存访问负载越高, 意味着扫描时间越长。如果仍然采用固定时间间隔进行扫描，可能没有办法适应不同工作负载大小的应用

![](./result/track/scan_time/scan_time.png)

#### 4.3.3 overhead

overhead 测试了 500 1000 2000 4000

T_interval = (SCAN_K * T_scan + SCAN_K_BASE) * T_scan + SCAN_BASE_INTERVAL

x = delte-x

t = (ax+b)x + c

16GB 负载算固定扫描间隔 C , 0-32Gb 重新确定一下 a b，x 是当前时刻负载和 16GB 负载扫描时间的差异数值

一张图, 横坐标负载，纵坐标 overhead 始终在 5% 以下 + 扫描间隔变化曲线

一张图，证明二次函数有效性

其中：  
- T_scan 代表本轮扫描所消耗的时间  
- SCAN_K 和 SCAN_K_BASE 是调节系数，用于控制扫描间隔对扫描时间的敏感度  
- SCAN_BASE_INTERVAL 是基础扫描间隔，保证低负载情况下仍然能进行周期性扫描  

- SCAN_K = 0.5
- SCAN_K_BASE = 50
- SCAN_BASE_INTERVAL = 2000

2000+ 

![](./result/track/overhead/overhead.png)


![](./result/track/pt_scan/pt_scan_redis_bar.png)

redis xsbench pr 都是访存比较密集的应用， graph500 的提升特别明显， 因为 Graph500 主要用于评测大规模图处理的性能，它的核心算法是 BFS（Breadth-First Search，广度优先搜索），Graph500 需要遍历大型图的数据结构（通常是邻接表或压缩格式存储），访问模式呈非连续、非局部特性, 这导致其访存模式是高度稀疏的

opt 的扫描模式会去除掉根节点的访问，大幅缩小扫描范围

![](./result/track/pt_scan/pt_scan_opt_all.png)

### page migration

CPU 绑定到 node0, 初始在 node2 上分配 2GB 内存，读写比例 7:3，不断随机访存 10000 次记录当前页面在 NUMA 节点上的分布情况

对比页面提升速度：页面提升由 numa_hint_fault 驱动，所有分层内存管理算法都在同一时间触发并开始进行 node2->node0 的页面提升, vtism 的页面提升速度是最快的

每个有 CPU 的顶层内存节点设置 4 个迁移线程，并且设置一对 wait_queue

![](./result/migration/promote/promote.png)

for 循环遍历一次所有内存，开一个线程每隔1s获取一次page的位置，画一个t的，看谁先迁移完成

```bash
numactl --cpubind=0 --membind=2 ./mm-mem/bin/cpu_loaded_latency
```

```bash
cat /proc/vmstat | grep numa
taskset -cp $(pidof cpu_loaded_latency)
numastat -p $(pidof cpu_loaded_latency)
```

## conclusion and future work

