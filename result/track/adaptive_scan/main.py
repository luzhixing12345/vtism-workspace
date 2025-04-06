import os
import re
import matplotlib.pyplot as plt

# ===== 可调样式参数 =====
bar_color = '#3333cb'
line_color = 'red'
font_size = 16
figsize = (12, 6)
dpi = 300
# =======================

log_dir = "."  # 当前目录
file_count = 24

x_labels = []
overhead = []
scan_times_ms = []

for i in range(1, file_count + 1):
    file_name = os.path.join(log_dir, f"{i}.log")
    if not os.path.exists(file_name):
        continue

    with open(file_name, "r") as f:
        content = f.read()

        # 匹配遍历时间
        times = re.findall(r"Sequential traversal completed in ([\d.]+) seconds", content)
        if len(times) >= 2:
            ori = float(times[0])
            scan = float(times[1])
            overhead_res = (scan - ori) / ori * 100
            overhead.append(overhead_res)
            x_labels.append(str(i))

            # 匹配扫描日志的时间，单位为 ms
            scan_entries = re.findall(rf"A/T: \d+/\d+ \(\d+ ms\) \[(\d+)\] \[{i} GB\]", content)
            if scan_entries:
                avg_scan_time = sum(map(int, scan_entries)) / len(scan_entries)
                scan_times_ms.append(avg_scan_time)
            else:
                scan_times_ms.append(0)
        else:
            print(f"Warning: {file_name} does not contain two traversal time entries.")

# ===== 绘图部分 =====
x = range(len(overhead))
fig, ax1 = plt.subplots(figsize=figsize)

# 柱状图 - Overhead (%)
ax1.bar(x, overhead, color=bar_color, label='Overhead (%)', edgecolor='black')
ax1.set_xlabel("Active Workload Memory Size(GB)", fontsize=font_size)
ax1.set_ylabel("Overhead (%)", fontsize=font_size, color='black')
ax1.tick_params(axis='y', labelcolor='black', labelsize=font_size)
ax1.tick_params(axis='x', labelsize=font_size)
ax1.set_xticks(x)
ax1.set_xticklabels(x_labels)
ax1.grid(axis='y', linestyle='--', alpha=0.5)
ax1.set_yticks([0, 5, 10, 15, 20])
ax1.axhline(y=5, color='gray', linestyle='--', linewidth=1)
# 折线图 - 扫描时间 (ms)
ax2 = ax1.twinx()
ax2.plot(x, scan_times_ms, color=line_color, marker='o', label='Scan Interval Time (ms)')
ax2.set_ylabel("Scan Interval Time (ms)", fontsize=font_size, color='black')
ax2.tick_params(axis='y', labelcolor='black', labelsize=font_size)

# 图例和布局
fig.tight_layout()
fig.legend(loc="upper left", bbox_to_anchor=(0.08, 0.95), fontsize=font_size)
plt.savefig("adaptive_scan.png", dpi=dpi)
# plt.show()
