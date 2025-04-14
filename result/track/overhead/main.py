import matplotlib.pyplot as plt
import re

# 样式配置
bar_color = "blue"
bar_width = 0.4
figsize = (10, 6)
dpi = 300

font_size_label = 22
font_size_ticks = 22

# 预定义的 benchmark 名称
benchmarks = ["redis", "pr", "graph500", "liblinear", "XSBench", "gups"]
benchmark_times = {name: [] for name in benchmarks}

# 读取日志内容并按块分组
with open("vtism-benchmark.log", "r") as f:
    lines = f.readlines()

# 每 3 行构成一个运行块：[benchmark], cmd, time
for i in range(0, len(lines), 3):
    if i + 2 >= len(lines):
        continue
    cmd_line = lines[i + 1].strip()
    time_line = lines[i + 2].strip()

    # 提取 benchmark 名
    matched = None
    for name in benchmarks:
        if name in cmd_line:
            matched = name
            break
    if not matched:
        continue

    # 提取时间
    match = re.search(r'time:\s*([\d.]+)', time_line)
    if match:
        t = float(match.group(1))
        benchmark_times[matched].append(t)
        print(f"matched {matched}")

# 计算 Overhead（第二次 - 第一次）/ 第一次 * 100
overheads = []
for name in benchmarks:
    times = benchmark_times[name]
    if len(times) >= 2:
        base, second = times[:2]
        if second < base:
            print(name, 'second < base')
        overhead = (second - base) / base * 100
        overheads.append(overhead)
    else:
        overheads.append(0)

# 画图
fig, ax = plt.subplots(figsize=figsize)
ax.bar(benchmarks, overheads, color=bar_color, edgecolor='black', width=bar_width)

ax.set_ylabel("Overhead (%)", fontsize=font_size_label)
ax.set_ylim(0, 8)
ax.set_yticks([0, 2, 4, 5, 6, 8])
ax.axhline(y=5, color='grey', linestyle='--', linewidth=2)

ax.set_xticks(range(len(benchmarks)))
ax.set_xticklabels(benchmarks, fontsize=font_size_ticks)
ax.tick_params(axis='y', labelsize=font_size_ticks)

ax.grid(axis='y', linestyle='--', alpha=0.5)

plt.tight_layout()
plt.savefig("benchmark_overhead.png", dpi=dpi)
# plt.show()
