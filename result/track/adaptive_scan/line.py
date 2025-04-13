import os
import re
import matplotlib.pyplot as plt

# 样式参数
line_colors = ['red', 'orange', 'green', 'blue']
font_size = 16
figsize = (10, 6)
dpi = 300

intervals = ['7', '8', '8+50', '9']
file_count = 24
full_indices = list(range(1, file_count + 1))

all_scan_time_full = []

# 提取 Scan Time 数据（1~24 全部）
for interval in intervals:
    scan_times = []
    for i in full_indices:
        file_name = os.path.join(interval, f"{i}.log")
        if not os.path.exists(file_name):
            scan_times.append(0)
            continue

        with open(file_name, "r") as f:
            content = f.read()
            times = re.findall(r"Sequential traversal completed in ([\d.]+) seconds", content)
            if len(times) >= 2:
                scan_entries = re.findall(rf"A/T: \d+/\d+ \(\d+ ms\) \[(\d+)\] \[{i} GB\]", content)
                if scan_entries:
                    avg_scan_time = sum(map(int, scan_entries)) / len(scan_entries)
                    scan_times.append(avg_scan_time)
                else:
                    scan_times.append(0)
            else:
                scan_times.append(0)
    all_scan_time_full.append(scan_times)

# 横坐标
x_full = list(range(file_count))
x_labels_full = [str(i + 1) for i in x_full]

# 画折线图
fig, ax = plt.subplots(figsize=figsize)
for idx, scan_time in enumerate(all_scan_time_full):
    ax.plot(x_full, scan_time, marker='o', color=line_colors[idx],
            label=f'Scan Time ({intervals[idx]})')

ax.set_xlabel("Active Workload Memory Size (GB)", fontsize=font_size)
ax.set_ylabel("Scan Interval Time (ms)", fontsize=font_size)
ax.set_xticks(x_full)
ax.set_xticklabels(x_labels_full)
ax.tick_params(axis='x', labelsize=font_size)
ax.tick_params(axis='y', labelsize=font_size)
ax.set_yticks([0, 30000, 60000, 90000, 120000])
ax.set_yticklabels(['0', '30000', '60000', '90000', '120000'])
ax.grid(axis='y', linestyle='--', alpha=0.5)

fig.tight_layout()
fig.legend(loc="upper left", bbox_to_anchor=(0.13, 0.95), fontsize=font_size)
plt.savefig("adaptive_scan_line.png", dpi=dpi)
# plt.show()
