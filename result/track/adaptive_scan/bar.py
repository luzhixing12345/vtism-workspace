import os
import re
import matplotlib.pyplot as plt

# 样式参数
bar_colors = ['#4c91b8', '#0064ab', '#ffc000', '#005083']
font_size = 16
figsize = (10, 6)
dpi = 300
bar_width = 0.35

intervals = ['7', '8', '8+50', '9']
file_count = 24
even_indices = list(range(2, file_count + 1, 2))

all_overhead = []

# 提取 Overhead 数据（偶数编号）
for interval in intervals:
    overhead = []
    for i in even_indices:
        file_name = os.path.join(interval, f"{i}.log")
        if not os.path.exists(file_name):
            overhead.append(0)
            continue

        with open(file_name, "r") as f:
            content = f.read()
            times = re.findall(r"Sequential traversal completed in ([\d.]+) seconds", content)
            if len(times) >= 2:
                ori = float(times[0])
                scan = float(times[1])
                overhead_res = (scan - ori) / ori * 100
                overhead.append(overhead_res)
            else:
                overhead.append(0)
    all_overhead.append(overhead)

# 横坐标
x = [i * 1.5 for i in range(len(even_indices))]
x_labels = [str(i) for i in even_indices]

# 画柱状图
fig, ax1 = plt.subplots(figsize=figsize)
for idx, overhead in enumerate(all_overhead):
    bar_pos = [i + (idx - 1.5) * bar_width for i in x]
    label = f'SCAN_K = {intervals[idx]}, SCAN_K_BASE = 0'
    if idx == 2:
        label = f'SCAN_K = 8, SCAN_K_BASE = 50'
    ax1.bar(bar_pos, overhead, width=bar_width,
            label=label, color=bar_colors[idx], edgecolor='black')

ax1.set_xlabel("Active Workload Memory Size (GB)", fontsize=font_size)
ax1.set_ylabel("Overhead (%)", fontsize=font_size)
ax1.set_xticks(x)
ax1.set_xticklabels(x_labels)
ax1.tick_params(axis='x', labelsize=font_size)
ax1.tick_params(axis='y', labelsize=font_size)
ax1.set_ylim(0, 10)
ax1.set_yticks([0, 5, 10])
ax1.axhline(y=5, color='gray', linestyle='--', linewidth=1)
ax1.grid(axis='y', linestyle='--', alpha=0.5)

fig.tight_layout()
fig.legend(loc="upper right", bbox_to_anchor=(0.98, 0.95), fontsize=font_size)
plt.savefig("adaptive_overhead_bar.png", dpi=dpi)
# plt.show()
